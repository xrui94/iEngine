# iEngine-Web ECS 与位掩码查询架构

本文说明 iEngine-Web 当前的 ECS（Entity-Component-System）设计，以及最近引入的位掩码组合查询（queryMask）的修改动机、与旧版的差异、性能对比和实现原理。

## 背景与目标
- 统一 ECS 查询接口，增强表达能力（AND/OR/NOT 组合）。
- 提升查询效率，减少遍历时的哈希查找与分支不确定性。
- 建立可扩展的组件类型管理（注册器），为后续优化与调试打下基础。

## 设计概述
### 世界与实体
- `WorldECS`：ECS 数据容器，仅存储原始组件，不承担行为。
- `EntityId`：实体的唯一标识（数值或字符串）。
- 组件存储：`Map<string, Map<EntityId, any>>`，按组件类型键分桶，每桶映射实体到组件实例。

### 系统与调度
- `SystemECS`：系统基类，定义 `initialize/update/destroy` 生命周期与优先级。
- `SystemManagerECS`：系统管理器，负责排序与逐帧调用。
- 现有系统：
  - `TransformSystem`：组装实体的世界矩阵，维护脏标记。
  - `LightSystem`：将 Transform 转换为光源的位姿与方向（根据类型）。
  - `RenderSystem`：收集 Mesh/Material/Layer，形成渲染队列缓存。

### 新的查询接口（唯一保留）
- `queryMask(criteria: { all?: string[]; any?: string[]; none?: string[] })`：
  - `all`：必须全部具备（AND）。
  - `any`：至少具备一个（OR）。
  - `none`：必须不具备（NOT）。
- 旧接口 `query(all: string[])` 已移除，所有系统均已迁移到 `queryMask`。

## 修改动机
- 旧版仅支持简单 AND（“必须包含这些组件”），表达力不足。
- 字符串键查询需要对每个实体做多次 `Map.has`，在多组件组合下成本高，CPU 预取与分支预测效果差。
- 需要统一的“过滤”语义以便常见的排除场景（如 `Hidden`/`Disabled`）。

## 差异与性能对比
### 旧版查询
- 形式：`query(['Transform', 'MeshECS', 'MaterialECS'])`
- 实现：对每个实体依次检查多个 `Map.has(id)`，满足条件则加入结果。
- 复杂度：近似 `O(E * K)`（E 为实体数，K 为组件键数），并伴随哈希查找与指针跳转。

### 位掩码查询
- 形式：`queryMask({ all: ['Transform', 'MeshECS'], none: ['Hidden'] })`
- 实现：
  - 使用注册器为每个组件类型分配一个位（bit）。
  - 每个实体维护一个整型掩码，表示其具备的组件集合。
  - 将查询条件合并为三个掩码（all/any/none），通过按位运算进行匹配。
- 复杂度：
  - 合并条件掩码一次性 `O(K)`；随后遍历实体时为常数次按位比较，整体近似 `O(E)`。
  - 按位运算在 CPU 上极快，分支可预测性更好。
- 额外开销：
  - 每个实体一个 `number` 掩码（当前为 32-bit），空间成本极低。
  - 组件类型第一次出现时懒注册一个位。

## 实现原理
### 组件类型注册器（`ComponentTypeRegistry`）
- 单例维护 `Map<ComponentKey, bitIndex>` 与自增 `nextBit`。
- `getMask(key)`：若未注册则分配新位并返回对应掩码 `1 << bitIndex`。
- `getCombinedMask(keys)`：对多键掩码取并集，用于 `all/any/none` 的条件合并。

### 实体掩码（`WorldECS.entityMask`）
- 在 `addComponent/removeComponent` 时更新实体掩码：
  - 添加：`mask = mask | bit`
  - 移除：`mask = mask & ~bit`
- 查询：
  - `all`：`(mask & allMask) === allMask`
  - `any`：`(mask & anyMask) !== 0`（若 `anyMask` 非 0）
  - `none`：`(mask & noneMask) === 0`

### 系统迁移与防御性过滤
- `TransformSystem`：`queryMask({ all: ['Transform'], none: ['Disabled', 'Hidden'] })`
- `LightSystem`：`queryMask({ all: ['Transform', 'LightECS'], none: ['Disabled', 'Hidden'] })`
- `RenderSystem`：`queryMask({ all: ['Transform', 'MeshECS', 'MaterialECS'], none: ['Disabled', 'Hidden'] })`
  - 额外防御：丢弃 `mesh` 或 `material` 为 `null/undefined` 的条目。

## 限制与扩展
- 位宽上限：当前采用 32-bit 掩码（最多 32 种组件类型）。
  - 若需要更多类型，建议将掩码实现切换为 `BigInt` 并调整位操作。
- 掩码仅表示“是否具备组件类型”，不涉及组件内部状态；当需要更细粒度过滤时，建议结合系统层面的业务规则与额外标记组件。

## 使用示例
```ts
// 排除隐藏或禁用实体，只要有 Transform 即可
const ids = world.queryMask({ all: ['Transform'], none: ['Hidden', 'Disabled'] });

// 宽松材质匹配：至少具备其中一种材质，且必须有 Mesh
const ids2 = world.queryMask({ all: ['MeshECS'], any: ['MaterialECS', 'DebugMaterial'] });

// 仅光源：需要 Transform，且拥有任一光源相关组件
const ids3 = world.queryMask({ all: ['Transform'], any: ['LightECS', 'DirectionalLightECS'] });
```

## 迁移说明
- 旧接口 `WorldECS.query(all)` 已移除，请统一使用 `queryMask`。
- 组件键仍为字符串，不需要显式预注册；注册器会懒注册位。

## 后续规划
- 将掩码扩展为 `BigInt`，支持更多组件类型与更复杂组合。
- 引入类型安全的组件键与系统级查询工具（预合并掩码、查询缓存）。
- 提供调试工具：可视化实体掩码、查询命中与过滤结果。
- 结合事件/系统时序，优化脏标记传播与主动批处理（减少不必要更新）。

## 混合架构边界与约束

本章给出在「SceneGraph(OOP) + ECS」混合模式下的边界、约束与落地规范，确保系统不存在双写/双驱动/双来源问题，同时维持渲染与交互的可扩展性与性能。

### 适用场景
- WebGIS/大规模静态或分层数据：瓦片/倾斜摄影/3D Tiles 等由 OOP 的 Layer/Tile 管理，负责拉取、缓存、LOD、批处理与裁剪。
- 动态与可系统化对象：角色、工具、动画、交互标注、UI 叠加等由 ECS 管理，负责变换、状态与系统更新。
- 两类对象在渲染阶段合流，使用统一的渲染数据结构与过滤语义。

### 边界原则
- 单一来源（Per-Type Single Source）：同一类别实体的运行态数据只能由 OOP 或 ECS 之一负责，不允许并行双来源。
- 单一路径（Single Path Update/Render）：每个类别的更新与渲染输入仅来自其所属来源，不做跨源回写或并行合并计算。
- 只读桥接（Read-only Bridging）：必要时允许从主来源将数据映射为另一侧的只读视图，用于工具/可视化，不参与运行态驱动。
- 统一契约（Unified Contract）：无论来源，进入渲染管线的数据结构一致（Renderable/Light 等），过滤标记一致（Hidden/Disabled 等）。

### 规范化约定
- Transform 权限边界：
  - GIS/OOP 内容：由 OOP 管理 Transform（含 LOD 与批处理坐标），ECS 不再对其写入，仅可读用于工具。
  - 动态/ECS 内容：由 ECS 的 TransformSystem 管理，OOP 不回写，不维护并行世界矩阵。
- 过滤语义一致：
  - `Hidden`：外观隐藏但仍可参与某些系统（如物理或选择）。
  - `Disabled`：禁用系统更新与渲染（完全排除）。
  - 两侧来源在进入渲染队列前统一应用 `none: ['Hidden','Disabled']` 过滤。
- 控制器输入单源：
  - 相机、交互控制输入只作用于一个主数据源（通常 OOP 的 View/Camera），ECS 侧仅使用其只读视图（如投影视锥）做系统判断。
- 渲染契约统一：
  - `Renderable` 结构规范（示例）：`{ mesh, material, worldMatrix, layerId?, sortKey?, batchHandle? }`
  - `Light` 结构规范：`{ type, position, direction, color, intensity, worldMatrix? }`
- 标准缓存分层：
  - `RenderableCacheOOP`：OOP 来源的可渲染项缓存（已应用过滤与批处理）。
  - `RenderableCacheECS`：ECS 来源的可渲染项缓存（由 RenderSystem 产出）。
  - 两者在 RenderGraph 的不同 Pass 合流，避免在收集阶段混合。
- Node↔Entity 映射：
  - 仅在需要跨域选择/定位时维护 `Scene.nodeToEntityId: Map<Node, EntityId>`；
  - ECS-first 的动态对象可选建立 Node 适配器（只读），OOP-first 的 GIS 对象不建立映射（避免双写）。

### 示例数据流
- OOP/GIS 内容（示例：瓦片与倾斜摄影）
  1) `LayerManager.update()`：确定可见层与层次。
  2) `TileScheduler.fetchAndCache()`：异步拉取瓦片、构建批处理资源。
  3) `Culler.cull(cameraFrustum)`：层内裁剪，生成 `RenderableCacheOOP`。
  4) `RenderGraph.run(MapLayerPass)`：消耗 `RenderableCacheOOP`，绘制批处理瓦片与模型。

- ECS/动态对象（示例：标注与角色）
  1) `SystemManagerECS.update()`：
     - `TransformSystem` 组装世界矩阵（排除 Hidden/Disabled）。
     - `LightSystem` 生成光源数据。
     - `RenderSystem.getRenderables()` 生成 `RenderableCacheECS`。
  2) `RenderGraph.run(ForwardPass)`：消耗 `RenderableCacheECS`，绘制动态对象。

- 相机与输入控制（单源）
  1) `CameraControllerOOP.update()`：更新视图矩阵与投影视锥。
  2) `SystemManagerECS` 只读使用 `cameraFrustum` 做可选系统判断（如可见性或交互）。

- 统一后处理（合流）
  1) `RenderGraph.run(PostProcessPass)`：对前述多个 Pass 的输出纹理做统一后处理与合成。

### 常见反模式与规避
- 双写/双算：同一对象的世界矩阵同时由 Node.updateWorldTransform 与 TransformSystem 写入。应选一侧为主，另一侧只读。
- 双驱动控制：相机/输入同时驱动 OOP 和 ECS。应仅驱动 OOP，相机状态以只读方式供 ECS 使用。
- 数据混收：在收集阶段将 OOP 与 ECS 渲染项混合在单列表再过滤。应在各自来源完成过滤与缓存，然后在 RenderGraph 层合流。
- 过度 ECS 化 GIS：将大批静态瓦片逐一映射为 ECS 实体导致管理开销与查询压力。应保持 OOP 的批处理与缓存主导。

### 最小集成清单
- 建立 `RenderableCacheOOP` 与 `RenderableCacheECS` 两个独立缓存。
- 在两个缓存入口统一应用 `none: ['Hidden','Disabled']` 过滤。
- RenderGraph 定义 `MapLayerPass`（OOP）与 `ForwardPass`（ECS），输出统一的帧图节点。
- 相机/输入只驱动 OOP，ECS 只读消费相机视锥。
- 按需维护 `Scene.nodeToEntityId`（仅用于选择/定位），禁止双向数据写入。
- 文档约定：明确每类对象的主来源及只读桥接策略，避免后续演进时出现隐性双来源。

## 核心概念与当前实现
- SceneGraph：层次化对象树，负责对象组织、生命周期与视图组合；在混合模式中更偏向“组织壳”，避免成为运行时多源数据的第二写入点。
- Node：图节点，包含本地变换、组件与子节点；当前 OOP 侧可维护 `Node.updateWorldTransform` 用于 GIS/Layer 批处理坐标。
- OOP：面向对象的组件与管理器（如 `LayerManager`, `TileScheduler`, `Culler`），负责数据拉取、缓存、LOD、批处理与裁剪，产出 `RenderableCacheOOP`。
- ECS：数据驱动的组件与系统（`WorldECS` + `TransformSystem`/`LightSystem`/`RenderSystem`），通过 `queryMask` 聚合与过滤，产出 `RenderableCacheECS`。
- 交互关系：
  - 渲染阶段合流：RenderGraph 以多个 Pass 消费两侧缓存。
  - 控制器单源：相机由 OOP 控制，ECS 只读消费视锥、相机矩阵。
  - 选择/定位：按需维护 `Scene.nodeToEntityId` 映射用于跨域选择，禁止双向写入（只读桥接）。

## RenderGraph 合流示意图
- 可视化文件：`docs/engine-arch.drawio`（建议以合流关系更新此图）。
- 文本示意：
```
[RenderableCacheOOP] --(MapLayerPass)--> [ColorOOP, Depth]
[RenderableCacheECS] --(ForwardPass)-->  [ColorECS] (reads Depth)
[ColorOOP, ColorECS] --(PostProcessPass)-->  [Present]
```
- 合流规则：
  - 两侧缓存都在各自入口统一应用 `none: ['Hidden','Disabled']` 过滤。
  - OOP 优先批处理/实例化、深度写入；ECS 复用深度减少重绘与排序成本。
  - 如需 G-buffer，OOP/ECS 可共享位置法线等缓冲，具体取决于后续管线设计。

## Pass 接口约定（最小伪代码）
```ts
// 统一数据契约（建议）
interface RenderContext { device: WebGLContext | WebGPUContext; frameGraph: FrameGraph; }
interface SceneView { scene: Scene; camera: Camera; viewport: Rect; }
interface Renderable { mesh: Mesh; material: Material; worldMatrix: Mat4; layerId?: number; sortKey?: number; batchHandle?: any; }
interface Light { type: 'dir'|'point'|'spot'; position: Vec3; direction?: Vec3; color: Vec3; intensity: number; }
interface Pass {
  name: string;
  setup(ctx: RenderContext, fg: FrameGraph): void; // 资源注册/声明依赖
  execute(ctx: RenderContext, view: SceneView): void; // 真正绘制
}

// OOP/GIS 合流入口：MapLayerPass
class MapLayerPass implements Pass {
  name = 'MapLayerPass';
  setup(ctx, fg) {
    fg.createTexture('ColorOOP', { format: 'rgba16f' });
    fg.createTexture('Depth', { format: 'depth24' });
  }
  execute(ctx, view) {
    const cache = view.scene.renderCacheOOP; // 已裁剪与批处理
    ctx.device.beginPass({ color: 'ColorOOP', depth: 'Depth', viewport: view.viewport });
    for (const r of cache) {
      // 批次/实例化优先，避免逐物体状态切换
      drawRenderable(ctx, r);
    }
    ctx.device.endPass();
  }
}

// ECS 动态对象入口：ForwardPass
class ForwardPass implements Pass {
  name = 'ForwardPass';
  setup(ctx, fg) {
    fg.readTexture('Depth');
    fg.createTexture('ColorECS', { format: 'rgba16f' });
  }
  execute(ctx, view) {
    const cache = view.scene.renderCacheECS; // 由 RenderSystem 生成
    ctx.device.beginPass({ color: 'ColorECS', depthRead: 'Depth', viewport: view.viewport });
    for (const r of cache) {
      drawRenderable(ctx, r);
    }
    ctx.device.endPass();
  }
}

// 合成/后处理
class PostProcessPass implements Pass {
  name = 'PostProcessPass';
  setup(ctx, fg) {
    fg.readTexture('ColorOOP');
    fg.readTexture('ColorECS');
    fg.createTexture('Present', { format: 'rgba8' });
  }
  execute(ctx, view) {
    compose(ctx, 'ColorOOP', 'ColorECS', 'Present');
    ctx.device.present('Present');
  }
}
```
- 约束摘要：
  - 输入缓存命名统一：`scene.renderCacheOOP` 与 `scene.renderCacheECS`。
  - 过滤一致：在缓存生成处应用 `Hidden/Disabled`，避免在 Pass 内重复判断。
  - 排序与批次：各自入口完成排序（`sortKey`）与批处理，Pass 内不再重排只做消耗。
  - 相机单源：`view.camera` 来自 OOP 控制器，ECS 只读消费视锥。

## README 链接索引建议
- 在 `iEngine-Web/README.md` 中添加以下索引以便导航：
  - `架构文档：docs/engine-arch.md`
  - `混合架构边界与约束：docs/engine-arch.md#混合架构边界与约束`
  - `核心概念与当前实现：docs/engine-arch.md#核心概念与当前实现`
  - `RenderGraph 合流示意图：docs/engine-arch.md#rendergraph-合流示意图`

## SceneGraph + Node + OOP 当前实现细节
- 文件结构与关联：
  - `src/core/Node.ts`：图节点，维护本地/世界变换、父子关系与组件集合。
  - `src/core/Component.ts`：组件基类，定义生命周期与节点绑定接口。
  - `src/renderers/RenderableComponent.ts`：可渲染组件，持有 `mesh/material/layer`，从节点读取世界矩阵。
  - `src/lights/LightComponent.ts`：光照组件，持有 `Light` 数据对象。
  - `src/scenes/World.ts`：轻量 OOP 视图扫描器，从节点树收集 `Renderable/Light`。
  - `src/scenes/Scene.ts`：场景根，维护 `_root: Node`、相机、OOP/ECS 世界与系统更新。
- Node 职责与行为：
  - 变换维护：`localPosition/localRotation/localScale` + `transformDirty` 标记；`getWorldTransform()` 在脏时调用 `updateWorldTransform()` 进行父子矩阵组合。
  - 层级管理：`addChild/removeChild/getChildren/getParent`；在添加/移除子节点时传递 `ownerScene` 引用与变换脏标记。
  - 组件管理：`components: Component[]`；`addComponent()` 绑定节点并调用 `onAttach()`；`removeComponent()` 调用 `onDetach()` 并解除绑定；`update(deltaTime)` 逐组件触发 `onUpdate()` 并递归更新子节点。
  - 设计约束：不内嵌渲染逻辑，仅提供世界变换与组件生命周期，便于未来 C++ 版本引入 ECS。
- Component 基类：
  - 标识与开关：`type: string`（用于类型识别）、`enabled: boolean`（启用/禁用）。
  - 节点绑定：`setNode/getNode` 存取所属节点；生命周期钩子包含 `onAttach/onDetach/onEnable/onDisable/onUpdate/onDestroy`。
  - 约定：组件自身不直接驱动渲染，主要承载数据与行为，由上层扫描/系统消费。
- RenderableComponent 关系与用法：
  - 数据：`mesh: Mesh`、`material: Material`、`layer: RenderLayerID`；`type = 'RenderableComponent'`。
  - 变换读取：`getWorldTransform()` 通过 `getNode()` 访问 `Node.getWorldTransform()` 并返回矩阵元素；自身不写入节点变换。
  - 消费路径：`World.scanScene.visitNode()` 在发现 `RenderableComponent` 时，构造 `Renderable` 条目 `{ mesh, material, worldTransform: node.getWorldTransform(), layer }` 推入 `renderables[]`。
- LightComponent 关系与用法：
  - 数据：`light: Light`；`type = 'LightComponent'`。
  - 行为：当前仅承载光数据，`onUpdate()` 预留扩展；位姿由节点变换或后续光系统按需推导。
  - 消费路径：`World.scanScene.visitNode()` 收集 `lc.light` 推入 `lights[]`。
- Scene 与 World 的 OOP 扫描流程：
  - 场景更新：`Scene.update(deltaTime)` 顺序为 `this._root.update(deltaTime)`（OOP 组件与节点递归更新）→ `this._ecsSystemManager.update(...)`（ECS 系统更新）→ `this._world.scanScene(this)`（构建 OOP 视图）。
  - 统一查询：`Scene.getRenderables()` 合并 `World.getRenderables()`（OOP）与 `RenderSystem.getRenderables()`（ECS）；`Scene.getLights()` 合并 OOP 的 `World.getLights()` 与 ECS 的光数组。
- 关系图（文字描述）：
  - `Node` 拥有 `Component[]`；`Node.update()` 驱动各组件的 `onUpdate()` 与变换传播。
  - `RenderableComponent` 继承 `Component`，读取 `Node.worldTransform`，对外提供 `Renderable` 数据条目。
  - `LightComponent` 继承 `Component`，对外提供 `Light` 数据条目。
  - `World.scanScene` 递归遍历节点树，消费 `RenderableComponent/LightComponent`，产出 `Renderable[]/Light[]` 给渲染器与管线。
- 使用与约束提醒：
  - OOP 侧的世界矩阵由 `Node.updateWorldTransform()` 驱动；ECS 侧由 `TransformSystem` 驱动，避免同一对象双写变换。
  - 组件的启用/禁用建议在扫描/系统阶段统一应用 `Hidden/Disabled` 过滤，保持两侧一致的过滤语义。
  - `ownerScene` 仅用于传递场景上下文引用，不作为双向数据通道，遵循只读桥接原则。

## 职责划分建议

- RenderView（视图本地）
  - 管理 viewport/scissor 、 camera 、清屏标志、背景/天空盒等视图参数。
  - 暴露 onBeforeRender / onAfterRender （每视图钩子）。
  - 持有该视图的后处理链（例如 Bloom/FXAA/Tonemap/OverlayUI ），以及每个 Pass 的启用、参数。
- Renderer（设备/管线）
  - 统一执行视图的渲染序列： setViewport → setScissor → clear → drawScene → postprocessChain → overlays 。
  - 实现 Pass 的绑定/执行、帧缓存/纹理/RT 的创建与复用、全局着色器资源缓存。
  - 可选的“全局”后处理（应用于最终合成，所有视图之后），例如调试叠加或性能采样。
- Engine（帧驱动/编排）
  - 帧时序与系统更新（动画、ECS、输入），多视图的执行顺序。
  - 提供帧级钩子（ onBeforeFrame / onAfterFrame ），区别于视图级钩子。
为什么这样分配更合理

- 多视图独立性：每个视图需要不同的清屏、曝光、色调映射或 UI 叠加，放在 RenderView 才能隔离。
- 资源与性能：Pass 的执行、RT 管理集中在 Renderer，更容易做资源复用与状态优化。
- 解耦与可测试：视图负责“配方”（配置），渲染器负责“烹饪”（执行），职责边界清晰。