# iEngine-web
iEngine for web(Test engine architecture). A tiny real-time graphics rendering engine based on webgl and webgpu.

## 文档索引
- 架构文档：../docs/engine-arch.md
- 混合架构边界与约束：../docs/engine-arch.md#混合架构边界与约束
- 核心概念与当前实现：../docs/engine-arch.md#核心概念与当前实现
- RenderGraph 合流示意图：../docs/engine-arch.md#rendergraph-合流示意图

## Getting Start

### Install Dependencies

Run the following command in the repo folder (e.g. iEngine-web)

```
npm install
```

### Build Library

For development environment (uncompressed, obfuscated code), Run:

```
npm run build-dev
```

For production environment，Run:

```
npm run build
```

Then, it will generate files at dist/

- iengine.js
- iengine.js.map
- types/... (Some *.d.ts types of files (for debugging))

### Run examples

Firstly, open the iEngine-web project using VSCode.
Then, open the "examples/index.html" file in VSCode.
Last, run it using the **"Live Server"** (author: Ritwick Dey) plugin of VSCode.

**Tip**: For more usage, please refer to the "examples/index.html" file.

## TODO

In the future, I will add the following designs:

- SceneLayer
- RenderLayer
- RenderGraph
- RenderPass
  - DepthPass
  - ShadowPass
  - PostProcessing
  - ...
- ...

Next, the general working logic of the engine will be updated as shown in the pseudocode below.

```js
Engine.frameLoop():
  for scene of this.scenes:
    renderer.render(scene)

Renderer.render(scene):
  const activeCamera = scene.activeCamera
  const context = scene.activeContext
  const renderGraph = scene.renderGraph

  // 每帧更新 Scene → RenderLayer（包括视锥裁剪、RenderQueue 构建）
  scene.updateRenderLayers()

  // 执行渲染图（按 Pass 顺序调度）
  renderGraph.execute(this, scene)

Scene.updateRenderLayers():
  for sceneLayer of this.sceneLayers:
    if (sceneLayer.visible):
      const renderLayers = sceneLayer.getRenderLayers()
      for rl of renderLayers:
        rl.collectRenderables() // 视锥裁剪 + push 到 RenderQueue

RenderGraph.execute(renderer, scene):
  for pass of this.passes:
    pass.execute(renderer, scene)

RenderPass.execute(renderer, scene):
  context.beginRenderPass(...)

  for renderLayer of scene.renderLayers:
    if (renderLayer.supportsPass(this.passType)):
      for renderable of renderLayer.renderQueue:
        this.drawRenderableInPass(renderable, renderer, scene)

  context.endRenderPass()

RenderPass.drawRenderableInPass(renderable, renderer, scene):
  const material = renderable.getMaterial()
  const mesh = renderable.getMesh()

  const passMaterial = material.getPassMaterial(this.passType)
  const shader = ShaderLib.getOrCompile(passMaterial.shaderName, passMaterial.getDefines())
  const pipeline = renderer.pipelineCache.getOrCreate(shader, passMaterial.getRenderState(), mesh.layout)

  pipeline.bind(context)
  mesh.uploadIfNeeded(context)

  shader.setUniforms(passMaterial.getUniforms(renderable, scene.camera, scene.lights))
  shader.setTextures(passMaterial.getTextures())

  context.draw(mesh, passMaterial.drawMode)
  pipeline.unbind?.()
```

我希望未来能满足如下使用场景：
- 多场景 ≠ 多 Canvas，但可能需要多 Renderer 实例
- “多场景”通常指：逻辑上多个独立的 3D 世界（如主游戏场景 + UI 场景 + 小地图场景）。
- 这些场景可以渲染到同一个 canvas 的不同区域（视口），也可以渲染到不同的 canvas（如画中画、多窗口预览）。

但当前 Engine 只持有一个 activeRenderer，且该 renderer 绑定唯一 canvas。

### RenderLayer
✅ 定义：
- 逻辑渲染任务单元：表示“用某个相机（Camera），把某个场景（Scene），渲染到 某个 canvas 的某个区域或整个区域”。
- 属于 应用层 / 架构层 的抽象，与具体 GPU 渲染技术无关。
- 一个 RenderLayer 可能包含：
  - 一个 Scene
  - 一个 Camera
  - 一个 Viewport（视口矩形）
  - 一个 clearColor
（可选）是否启用后处理链

Engine 管理并持有1个AssetManager、多个 Scene 和 RenderView。
每个 Scene 可以有多个 RenderView，每个 RenderView 只能对应一个 Scene。
或 1个RenderView 可以引用多个 Scene，

Scene 只是纯数据容器（含 OOP 数据 和 ECS 数据）

[Engine]
  └─ RenderView
        ├─ RenderLayer A  ← 架构层：渲染主场景
        │     └─ Renderer.render(scene, camera)
        │           └─ [内部执行]
        │                 ├─ Geometry Pass（G-buffer / forward）
        │                 └─ PostProcess Chain（多个 GPU Pass）
        │
        └─ RenderLayer B  ← 架构层：渲染小地图
              └─ Renderer.render(minimapScene, minimapCamera)
                    └─ [内部可能无后处理，或简单后处理]

最新架构设计：
[Application Layer]        ← 用户代码（*.html）
   ├─ Scene A (main world)
   ├─ Scene B (UI)
   └─ Scene C (minimap)

[Composition Layer]        ← 负责“谁渲染到哪”
   ├─ RenderView 1 → renders Scene A to viewport (0,0,1024,768)
   ├─ RenderView 2 → renders Scene C to viewport (10,10,200,200)
   └─ RenderView 3 → renders Scene B to full viewport (overlay)

[Rendering Pipeline Layer] ← 负责“如何渲染一个场景”
   └─ Each RenderView uses:
        ├─ Optional RenderPass[] (e.g. [ForwardPass, BloomPass])
        └─ Or simple Renderer.render(scene, camera) for basic cases