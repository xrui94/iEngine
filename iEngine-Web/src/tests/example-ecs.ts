// ECS架构使用示例
import { Node } from '../core/Node';
import { RenderableComponent } from '../renderers/RenderableComponent';
import { LightComponent } from '../lights/LightComponent';
import { Mesh } from '../core/Mesh';
import { Material } from '../materials/Material';
import { Light } from '../lights/Light';
import { RenderLayerID } from '../renderers/RenderLayer';

// 创建一个实体
const entity = new Node('MyEntity');

// 创建一个网格和材质（这里只是示例）
// const mesh = new Mesh(); // 实际使用时需要传入几何体数据
// const material = new Material(); // 实际使用时需要传入材质参数

// 创建一个渲染组件并添加到实体
// const renderableComponent = new RenderableComponent(mesh, material, RenderableLayerID.Opaque);
// entity.addComponent(renderableComponent);

// 创建一个光源并添加到实体
// const light = new Light(); // 实际使用时需要设置光源参数
// const lightComponent = new LightComponent(light);
// entity.addComponent(lightComponent);

// 创建子实体并添加到父实体
const childEntity = new Node('ChildEntity');
entity.addChild(childEntity);

// 更新实体及其所有组件和子实体
// entity.update(0.016); // 假设是60FPS，deltaTime为0.016秒

console.log('ECS示例创建成功');