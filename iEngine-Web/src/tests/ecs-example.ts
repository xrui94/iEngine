// ECS架构使用示例
import { Engine } from '../core/Engine';
import { Scene } from '../scenes/Scene';
import { Node } from '../core/Node';
import { RenderableComponent } from '../renderers/RenderableComponent';
import { LightComponent } from '../lights/LightComponent';
import { RenderSystem } from '../renderers/RenderSystem';

// 创建引擎
const engine = new Engine({
    renderer: 'webgl'
});

// 启动引擎
engine.start();

// 创建场景
// const canvas = document.getElementById('canvas') as HTMLCanvasElement;
// const scene = new Scene(canvas, {
//     useWebGL1: false,
//     attrs: {
//         antialias: true,
//         alpha: true
//     }
// });

// 将场景添加到引擎
// engine.addScene('main', scene);

// // 设置活动场景
// engine.setActiveScene('main');

// // 创建实体
// const entity = new Node('MyEntity');

// // 创建子实体
// const childEntity = new Node('ChildEntity');
// entity.addChild(childEntity);

// // 添加组件到实体
// // 注意：这里需要实际的Mesh和Material实例
// // const renderableComponent = new RenderableComponent(mesh, material);
// // entity.addComponent(renderableComponent);

// // 将实体添加到场景
// // scene.addEntity(entity);

// // 获取渲染系统并设置场景
// const renderSystem = engine.getRenderSystem();
// // renderSystem.setScene(scene);

// // 动画循环
// engine.setAnimationLoop(() => {
//     engine.tick();
// });

console.log('ECS架构示例设置完成');