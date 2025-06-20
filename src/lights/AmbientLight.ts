import { Color } from '../materials/Color';
import { Light } from './Light';


export type AmbientLightOptions = {
    color: Color | [number, number, number],
    intensity: number
}

export class AmbientLight extends Light {
    constructor({
        color = Color.white,
        intensity = 1.0
    }: AmbientLightOptions) {
        super({
            type: 'ambient',
            color,
            intensity
        });
    }
}
