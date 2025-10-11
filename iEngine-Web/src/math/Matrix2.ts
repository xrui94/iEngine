export class Matrix2 {
    public elements: Float32Array;

    /**
     * 构造函数，初始化矩阵元素
     * 
     * @param elements 可选参数，包含4个元素的数组，用于直接初始化矩阵
     */
    constructor(elements?: number[]) {
        if (elements && elements.length === 4) {
            // 当提供了一个长度为4的数组时，使用该数组初始化矩阵元素
            this.elements = new Float32Array(elements);
        } else {
            // 如果未提供数组或数组长度不为4，则默认初始化为2x2的单位矩阵
            this.elements = new Float32Array([
                1, 0,
                0, 1
            ]);
        }
    }

    static identity(): Matrix2 {
        return new Matrix2([
            1, 0,
            0, 1
        ]);
    }

    clone(): Matrix2 {
        return new Matrix2(Array.from(this.elements));
    }

    copy(m: Matrix2): this {
        this.elements.set(m.elements);
        return this;
    }

    multiply(matrix: Matrix2): Matrix2 {
        const a = this.elements;
        const b = matrix.elements;
        const result = new Matrix2();

        result.elements[0] = a[0] * b[0] + a[1] * b[2];
        result.elements[1] = a[0] * b[1] + a[1] * b[3];
        result.elements[2] = a[2] * b[0] + a[3] * b[2];
        result.elements[3] = a[2] * b[1] + a[3] * b[3];

        return result;
    }

    determinant(): number {
        return this.elements[0] * this.elements[3] - this.elements[1] * this.elements[2];
    }

    transpose(): Matrix2 {
        const result = this.clone();
        const temp = result.elements[1];
        result.elements[1] = result.elements[2];
        result.elements[2] = temp;
        return result;
    }
}