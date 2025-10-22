// ECS架构测试
import { Node } from '../core/Node';
import { Component } from '../core/Component';
import { RenderableComponent } from '../renderers/RenderableComponent';
import { RenderLayerID } from '../renderers/RenderLayer';

// 测试组件
class TestComponent extends Component {
    readonly type = 'TestComponent';
    
    public testData: string = 'test';
    
    onUpdate(deltaTime: number): void {
        console.log(`TestComponent updated with deltaTime: ${deltaTime}`);
    }
}

// 测试ECS架构
function testECS() {
    console.log('=== ECS架构测试 ===');
    
    // 1. 创建实体
    const entity = new Node('TestEntity');
    console.log('创建实体:', entity.name);
    
    // 2. 创建组件
    const testComponent = new TestComponent();
    testComponent.testData = 'Hello ECS!';
    
    // 3. 将组件添加到实体
    entity.addComponent(testComponent);
    console.log('添加组件到实体');
    
    // 4. 获取组件
    const retrievedComponent = entity.getComponent(TestComponent);
    console.log('获取组件:', retrievedComponent?.testData);
    
    // 5. 创建子实体
    const childEntity = new Node('ChildEntity');
    entity.addChild(childEntity);
    console.log('创建子实体:', childEntity.name);
    
    // 6. 更新实体（会自动更新所有组件）
    entity.update(0.016); // 模拟60FPS
    
    console.log('=== ECS架构测试完成 ===');
}

// 运行测试
testECS();

export { testECS };