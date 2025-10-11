import { Texture } from './Texture';
import { TextureOptions } from './Texture';

export class Texture3D extends Texture {
    // private _width: number;
    // private _height: number;
    private _depth: number;

    constructor(
        options: TextureOptions & { name: string, width?: number, height?: number, depth?: number }
    ) {
        super(options);
        // this._width = options.width ?? 64;
        // this._height = options.height ?? 64;
        this.width = options.width ?? 64;
        this.height = options.height ?? 64;
        this._depth = options.depth ?? 64;
    }

    // override get width(): number {
    //     return this._width;
    // }

    // override set width(value: number) {
    //     this._width = value;
    //     this._needsUpdate = true;
    // }

    // override get height(): number {
    //     return this._height;
    // }

    // override set height(value: number) {
    //     this._height = value;
    //     this._needsUpdate = true;
    // }

    get depth(): number {
        return this._depth;
    }

    set depth(value: number) {
        this._depth = value;
        this._needsUpdate = true;
    }

    override needsUpdate(): boolean {
        return super.needsUpdate() || this._needsUpdate;
    }
}