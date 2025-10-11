export class Color {
    r: number;
    g: number;
    b: number;
    a: number;

    x: number;
    y: number;
    z: number;
    w: number;

    constructor(r: number = 1, g: number = 1, b: number = 1) {
        this.r = r;
        this.g = g;
        this.b = b;
        this.a = 1.0;

        this.x = this.r;
        this.y = this.g;
        this.z = this.b;
        this.w = this.a;
    }

    // 常见颜色常量
    static get white() { return new Color(1, 1, 1); }
    static get black() { return new Color(0, 0, 0); }
    static get red()   { return new Color(1, 0, 0); }
    static get green() { return new Color(0, 1, 0); }
    static get blue()  { return new Color(0, 0, 1); }
    static get yellow(){ return new Color(1, 1, 0); }
    static get cyan()  { return new Color(0, 1, 1); }
    static get magenta(){ return new Color(1, 0, 1); }
    static get gray()  { return new Color(0.5, 0.5, 0.5); }

    set(r: number, g: number, b: number): this {
        this.r = r;
        this.g = g;
        this.b = b;

        this.x = this.r;
        this.y = this.g;
        this.z = this.b;
        this.w = this.a;

        return this;
    }
    
    get length(): number {
        return 3;
    }

    clone(): Color {
        return new Color(this.r, this.g, this.b);
    }

    copy(color: Color): this {
        this.r = color.r;
        this.g = color.g;
        this.b = color.b;
        return this;
    }

    toArray(): [number, number, number] {
        return [this.r, this.g, this.b];
    }

    multiplyScalar(s: number): this {
        this.r *= s;
        this.g *= s;
        this.b *= s;
        return this;
    }

    equals(color: Color): boolean {
        return this.r === color.r && this.g === color.g && this.b === color.b;
    }

}