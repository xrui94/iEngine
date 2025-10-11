import { Color } from '../materials/Color';
import { Vector3 } from '../math/Vector3';
import { Light } from './Light';

export type SpotLightOptions = {
    position: Vector3;  // 光源位置
    direction: Vector3;  // 光照方向
    angle: number;   // 光照角度
    range: number;   // 光照范围
    color: Color;       //
    intensity: number;  //
}

export class SpotLight extends Light {
    public position: Vector3;   // 光源位置
    public direction: Vector3;  // 光照方向
    public angle: number;   // 光照角度
    public range: number;   // 光照范围

    constructor({
        color = Color.white,
        intensity = 1.0,
        position = new Vector3(0, 10, 0),
        direction = new Vector3(0, -1, 0),
        angle = Math.PI / 6,
        range = 100
    }: SpotLightOptions) {
        super({
            type: 'spot',
            color,
            intensity
        });
        this.position = position;
        this.direction = direction;
        this.angle = angle;
        this.range = range;
    }
}