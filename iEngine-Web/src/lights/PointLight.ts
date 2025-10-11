import { Color } from '../materials/Color';
import { Vector3 } from '../math/Vector3';
import { Light } from './Light';

export type PointLightOptions = {
    position: Vector3;  // 光源位置
    color: Color;       //
    intensity: number;  //
    range: number;      // 光照
}

export class PointLight extends Light {
    public position: Vector3;   // 光源位置
    public range: number;   // 光照范围

    constructor({
        color = Color.white,
        intensity = 1.0,
        position = new Vector3(0, 10, 0),
        range = 100
    } : PointLightOptions) {
        super({
            type: 'point',
            color,
            intensity
        });
        this.position = position;
        this.range = range;
    }
}