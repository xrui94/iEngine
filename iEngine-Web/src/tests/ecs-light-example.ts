// ECS光源使用示例
import { Node } from '../core/Node';
import { LightComponent } from '../lights/LightComponent';
import { AmbientLight } from '../lights/AmbientLight';
import { DirectionalLight } from '../lights/DirectionalLight';
import { PointLight } from '../lights/PointLight';
import { SpotLight } from '../lights/SpotLight';
import { Color } from '../materials/Color';
import { Vector3 } from '../math/Vector3';

// 创建环境光实体
function createAmbientLightEntity() {
    const entity = new Node('AmbientLight');
    
    // 创建环境光
    const ambientLight = new AmbientLight({
        color: new Color(0.3, 0.3, 0.3),
        intensity: 1.0
    });
    
    // 创建光源组件并添加到实体
    const lightComponent = new LightComponent(ambientLight);
    entity.addComponent(lightComponent);
    
    return entity;
}

// 创建方向光实体
function createDirectionalLightEntity() {
    const entity = new Node('DirectionalLight');
    
    // 创建方向光
    const directionalLight = new DirectionalLight({
        color: new Color(1.0, 1.0, 1.0),
        intensity: 5.0,
        position: new Vector3(0, 10, 0),
        direction: new Vector3(0, -1, 0),
        castShadow: false,
        shadowBias: 0.0
    });
    
    // 创建光源组件并添加到实体
    const lightComponent = new LightComponent(directionalLight);
    entity.addComponent(lightComponent);
    
    return entity;
}

// 创建点光源实体
function createPointLightEntity() {
    const entity = new Node('PointLight');
    
    // 创建点光源
    const pointLight = new PointLight({
        color: new Color(1.0, 0.5, 0.0),
        intensity: 3.0,
        position: new Vector3(5, 5, 5),
        range: 50
    });
    
    // 创建光源组件并添加到实体
    const lightComponent = new LightComponent(pointLight);
    entity.addComponent(lightComponent);
    
    return entity;
}

// 创建聚光灯实体
function createSpotLightEntity() {
    const entity = new Node('SpotLight');
    
    // 创建聚光灯
    const spotLight = new SpotLight({
        color: new Color(0.0, 1.0, 1.0),
        intensity: 4.0,
        position: new Vector3(0, 10, 0),
        direction: new Vector3(0, -1, 0),
        angle: Math.PI / 6,
        range: 30
    });
    
    // 创建光源组件并添加到实体
    const lightComponent = new LightComponent(spotLight);
    entity.addComponent(lightComponent);
    
    return entity;
}

// 使用示例
function example() {
    console.log('=== ECS光源示例 ===');
    
    // 创建各种光源实体
    const ambientEntity = createAmbientLightEntity();
    const directionalEntity = createDirectionalLightEntity();
    const pointEntity = createPointLightEntity();
    const spotEntity = createSpotLightEntity();
    
    console.log('创建了以下光源实体:');
    console.log('- 环境光实体:', ambientEntity.name);
    console.log('- 方向光实体:', directionalEntity.name);
    console.log('- 点光源实体:', pointEntity.name);
    console.log('- 聚光灯实体:', spotEntity.name);
    
    // 可以将这些实体添加到场景中
    // scene.addEntity(ambientEntity);
    // scene.addEntity(directionalEntity);
    // scene.addEntity(pointEntity);
    // scene.addEntity(spotEntity);
    
    console.log('=== ECS光源示例完成 ===');
}

// 运行示例
example();

export { 
    createAmbientLightEntity, 
    createDirectionalLightEntity, 
    createPointLightEntity, 
    createSpotLightEntity 
};