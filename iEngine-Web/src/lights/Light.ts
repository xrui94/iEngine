import { Vector3 } from '../math/Vector3';
import { Color } from '../materials/Color';

export type LightType = 'ambient' | 'directional' | 'point' | 'spot';

export type LightOptions = {
    type: LightType,
    color: Color | [number, number, number],
    intensity: number
}

export class Light {
    public type: LightType; // 光源类型
    public _color: Color = new Color(1.0, 1.0, 1.0); // RGB
    public _intensity: number = 1.0; // 光照强度

    constructor({
        type,
        color,
        intensity = 1.0
    }: LightOptions) {
        this.type = type;
        if (color) this.color = color;
        this._intensity = intensity;
    }

    get color(): Color {
        return this._color;
    }

    set color(value: Color | [number, number, number]) {
        if (value instanceof Color) {
            this._color = value;
        } else {
            this._color = new Color(...value);
        }
    }

    get intensity(): number {
        return this._intensity;
    }

    set intensity(intensity: number) {
        this._intensity = intensity;
    }

}