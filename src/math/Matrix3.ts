export class Matrix3 {
    public elements: Float32Array;

    constructor(elements?: number[]) {
        if (elements && elements.length === 9) {
            this.elements = new Float32Array(elements);
        } else {
            // 单位矩阵
            this.elements = new Float32Array([
                1, 0, 0,
                0, 1, 0,
                0, 0, 1
            ]);
        }
    }

    get length(): number {
        return this.elements.length;
    }

    static identity(): Matrix3 {
        return new Matrix3([
            1, 0, 0,
            0, 1, 0,
            0, 0, 1
        ]);
    }

    clone(): Matrix3 {
        return new Matrix3(Array.from(this.elements));
    }

    copy(m: Matrix3): this {
        this.elements.set(m.elements);
        return this;
    }
}