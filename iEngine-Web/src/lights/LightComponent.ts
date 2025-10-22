import { Component } from '../core/Component';
import { Light } from './Light';

export class LightComponent extends Component {
    readonly type = 'LightComponent';
    
    constructor(public light: Light) {
        super();
    }
    
    onUpdate(deltaTime: number): void {
        // 光照组件更新逻辑
    }
}