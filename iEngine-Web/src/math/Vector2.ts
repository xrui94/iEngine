export class Vector2 {
    constructor(
        public x: number = 0,
        public y: number = 0
    ) {}
    
    set(x: number, y: number): this {
        this.x = x;
        this.y = y;
        return this;
    }

    toArray(): number[] {
        return [this.x, this.y];
    }
    
    copy(v: Vector2): this {
        this.x = v.x;
        this.y = v.y;
        return this;
    }
    
    add(v: Vector2): this {
        this.x += v.x;
        this.y += v.y;
        return this;
    }
    
    sub(v: Vector2): this {
        this.x -= v.x;
        this.y -= v.y;
        return this;
    }
    
    multiplyScalar(scalar: number): this {
        this.x *= scalar;
        this.y *= scalar;
        return this;
    }
    
    divideScalar(scalar: number): this {
        return this.multiplyScalar(1 / scalar);
    }
    
    length(): number {
        return Math.sqrt(this.x * this.x + this.y * this.y);
    }
    
    normalize(): this {
        return this.divideScalar(this.length() || 1);
    }
    
    dot(v: Vector2): number {
        return this.x * v.x + this.y * v.y;
    }
    
    distanceTo(v: Vector2): number {
        return Math.sqrt(
            (this.x - v.x) ** 2 +
            (this.y - v.y) ** 2
        );
    }
}