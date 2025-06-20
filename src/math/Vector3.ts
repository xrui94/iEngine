export class Vector3 {
    constructor(
        public x: number = 0,
        public y: number = 0,
        public z: number = 0
    ) {}
    
    set(x: number, y: number, z: number): this {
        this.x = x;
        this.y = y;
        this.z = z;
        return this;
    }

    toArray(): number[] {
        return [this.x, this.y, this.z];
    }
    
    copy(v: Vector3): this {
        this.x = v.x;
        this.y = v.y;
        this.z = v.z;
        return this;
    }
    
    add(v: Vector3): this {
        this.x += v.x;
        this.y += v.y;
        this.z += v.z;
        return this;
    }
    
    sub(v: Vector3): this {
        this.x -= v.x;
        this.y -= v.y;
        this.z -= v.z;
        return this;
    }
    
    multiplyScalar(scalar: number): this {
        this.x *= scalar;
        this.y *= scalar;
        this.z *= scalar;
        return this;
    }
    
    divideScalar(scalar: number): this {
        return this.multiplyScalar(1 / scalar);
    }
    
    length(): number {
        return Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
    }
    
    normalize(): this {
        return this.divideScalar(this.length() || 1);
    }
    
    dot(v: Vector3): number {
        return this.x * v.x + this.y * v.y + this.z * v.z;
    }
    
    cross(v: Vector3): Vector3 {
        return new Vector3(
            this.y * v.z - this.z * v.y,
            this.z * v.x - this.x * v.z,
            this.x * v.y - this.y * v.x
        );
    }
    
    distanceTo(v: Vector3): number {
        return Math.sqrt(
            (this.x - v.x) ** 2 +
            (this.y - v.y) ** 2 +
            (this.z - v.z) ** 2
        );
    }
}