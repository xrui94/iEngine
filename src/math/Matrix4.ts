import { Matrix3 } from "./Matrix3";

export class Matrix4 {
    public elements: Float32Array;
    
    constructor(elements?: number[]) {
        if (elements && elements.length === 16) {
            this.elements = new Float32Array(elements);
        } else {
            // 单位矩阵
            this.elements = new Float32Array([
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            ]);
        }
    }

    static identity(): Matrix4 {
        return new Matrix4([
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]);
    }

    static fromTranslation(offset: { x: number; y: number; z: number }): Matrix4 {
        return new Matrix4([
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            offset.x, offset.y, offset.z, 1
        ]);
    }

    static fromScaling(scale: { x: number; y: number; z: number }): Matrix4 {
        return new Matrix4([
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1
        ]);
    }

    static fromRotation(axis: { x: number; y: number; z: number }, angleRad: number): Matrix4 {
        const x = axis.x, y = axis.y, z = axis.z;
        const len = Math.sqrt(x * x + y * y + z * z);
        if (len === 0) return Matrix4.identity();
        const nx = x / len, ny = y / len, nz = z / len;
        const c = Math.cos(angleRad);
        const s = Math.sin(angleRad);
        const t = 1 - c;

        return new Matrix4([
            t * nx * nx + c,     t * nx * ny - s * nz, t * nx * nz + s * ny, 0,
            t * nx * ny + s * nz, t * ny * ny + c,     t * ny * nz - s * nx, 0,
            t * nx * nz - s * ny, t * ny * nz + s * nx, t * nz * nz + c,     0,
            0, 0, 0, 1
        ]);
    }

    toMatrix3(): Matrix3 {
        const m = this.elements;
        // 提取左上3x3
        return new Matrix3([
            m[0], m[1], m[2],
            m[4], m[5], m[6],
            m[8], m[9], m[10]
        ]);
    }
    
    identity(): this {
        this.elements.set([
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]);
        return this;
    }
    
    multiply(other: Matrix4): this {
        const a = this.elements;
        const b = other.elements;
        const result = new Float32Array(16);
        
        for (let i = 0; i < 4; i++) {
            for (let j = 0; j < 4; j++) {
                result[j * 4 + i] = 0;
                for (let k = 0; k < 4; k++) {
                    result[j * 4 + i] += a[k * 4 + i] * b[j * 4 + k];
                }
            }
        }
        
        this.elements = result;
        return this;
    }

    inverse(): this {
        const m = this.elements;
        const inv = new Float32Array(16);

        inv[0]  =  m[5] * m[10] * m[15] - 
                   m[5] * m[11] * m[14] - 
                   m[9] * m[6] * m[15] + 
                   m[9] * m[7] * m[14] +
                   m[13] * m[6] * m[11] - 
                   m[13] * m[7] * m[10];

        inv[4]  = -m[4] * m[10] * m[15] + 
                   m[4] * m[11] * m[14] + 
                   m[8] * m[6] * m[15] - 
                   m[8] * m[7] * m[14] - 
                   m[12] * m[6] * m[11] + 
                   m[12] * m[7] * m[10];

        inv[8]  =  m[4] * m[9] * m[15] - 
                   m[4] * m[11] * m[13] - 
                   m[8] * m[5] * m[15] + 
                   m[8] * m[7] * m[13] + 
                   m[12] * m[5] * m[11] - 
                   m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] + 
                   m[4] * m[10] * m[13] +
                   m[8] * m[5] * m[14] - 
                   m[8] * m[6] * m[13] - 
                   m[12] * m[5] * m[10] + 
                   m[12] * m[6] * m[9];

        inv[1]  = -m[1] * m[10] * m[15] + 
                   m[1] * m[11] * m[14] + 
                   m[9] * m[2] * m[15] - 
                   m[9] * m[3] * m[14] - 
                   m[13] * m[2] * m[11] + 
                   m[13] * m[3] * m[10];

        inv[5]  =  m[0] * m[10] * m[15] - 
                   m[0] * m[11] * m[14] - 
                   m[8] * m[2] * m[15] + 
                   m[8] * m[3] * m[14] + 
                   m[12] * m[2] * m[11] - 
                   m[12] * m[3] * m[10];

        inv[9]  = -m[0] * m[9] * m[15] + 
                   m[0] * m[11] * m[13] + 
                   m[8] * m[1] * m[15] - 
                   m[8] * m[3] * m[13] - 
                   m[12] * m[1] * m[11] + 
                   m[12] * m[3] * m[9];

        inv[13] =  m[0] * m[9] * m[14] - 
                   m[0] * m[10] * m[13] - 
                   m[8] * m[1] * m[14] + 
                   m[8] * m[2] * m[13] + 
                   m[12] * m[1] * m[10] - 
                   m[12] * m[2] * m[9];

        inv[2]  =  m[1] * m[6] * m[15] - 
                   m[1] * m[7] * m[14] - 
                   m[5] * m[2] * m[15] + 
                   m[5] * m[3] * m[14] + 
                   m[13] * m[2] * m[7] - 
                   m[13] * m[3] * m[6];

        inv[6]  = -m[0] * m[6] * m[15] + 
                   m[0] * m[7] * m[14] + 
                   m[4] * m[2] * m[15] - 
                   m[4] * m[3] * m[14] - 
                   m[12] * m[2] * m[7] + 
                   m[12] * m[3] * m[6];

        inv[10] =  m[0] * m[5] * m[15] - 
                   m[0] * m[7] * m[13] - 
                   m[4] * m[1] * m[15] + 
                   m[4] * m[3] * m[13] + 
                   m[12] * m[1] * m[7] - 
                   m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] + 
                   m[0] * m[6] * m[13] + 
                   m[4] * m[1] * m[14] - 
                   m[4] * m[2] * m[13] - 
                   m[12] * m[1] * m[6] + 
                   m[12] * m[2] * m[5];

        inv[3]  = -m[1] * m[6] * m[11] + 
                   m[1] * m[7] * m[10] + 
                   m[5] * m[2] * m[11] - 
                   m[5] * m[3] * m[10] - 
                   m[9] * m[2] * m[7] + 
                   m[9] * m[3] * m[6];

        inv[7]  =  m[0] * m[6] * m[11] - 
                   m[0] * m[7] * m[10] - 
                   m[4] * m[2] * m[11] + 
                   m[4] * m[3] * m[10] + 
                   m[8] * m[2] * m[7] - 
                   m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] + 
                   m[0] * m[7] * m[9] + 
                   m[4] * m[1] * m[11] - 
                   m[4] * m[3] * m[9] - 
                   m[8] * m[1] * m[7] + 
                   m[8] * m[3] * m[5];

        inv[15] =  m[0] * m[5] * m[10] - 
                   m[0] * m[6] * m[9] - 
                   m[4] * m[1] * m[10] + 
                   m[4] * m[2] * m[9] + 
                   m[8] * m[1] * m[6] - 
                   m[8] * m[2] * m[5];

        let det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (det === 0) {
            throw new Error("Matrix4: can't invert, determinant is 0");
        }

        det = 1.0 / det;
        for (let i = 0; i < 16; i++) {
            inv[i] *= det;
        }
        this.elements = inv;
        return this;
    }

    transpose(): this {
        const m = this.elements;
        const t = new Float32Array(16);
        for (let i = 0; i < 4; i++) {
            for (let j = 0; j < 4; j++) {
                t[i * 4 + j] = m[j * 4 + i];
            }
        }
        this.elements = t;
        return this;
    }

    lookAt(eyeX: number, eyeY: number, eyeZ: number, 
        targetX: number, targetY: number, targetZ: number,
        upX: number, upY: number, upZ: number): this {
        
        // 计算前向向量
        let fx = targetX - eyeX;
        let fy = targetY - eyeY;
        let fz = targetZ - eyeZ;
        
        // 归一化前向向量
        let flen = Math.sqrt(fx * fx + fy * fy + fz * fz);
        if (flen === 0) {
            // 如果目标点和眼睛重合，则默认朝-z
            fz = -1;
            flen = 1;
        }
        const fxn = fx / flen;
        const fyn = fy / flen;
        const fzn = fz / flen;
        
        // 计算右向量 (前向 x 上方向)
        let rx = fyn * upZ - fzn * upY;
        let ry = fzn * upX - fxn * upZ;
        let rz = fxn * upY - fyn * upX;
        
        // 归一化右向量
        let rlen = Math.sqrt(rx * rx + ry * ry + rz * rz);
        if (rlen === 0) {
            // 如果 up 向量与前向共线，选择一个默认up
            if (Math.abs(upZ) === 1) {
                rx = 1; ry = 0; rz = 0; rlen = 1;
            } else {
                rx = 0; ry = 0; rz = 1; rlen = 1;
            }
        }
        const rxn = rx / rlen;
        const ryn = ry / rlen;
        const rzn = rz / rlen;
        
        // 计算上向量 (右向量 x 前向)
        const ux = ryn * fzn - rzn * fyn;
        const uy = rzn * fxn - rxn * fzn;
        const uz = rxn * fyn - ryn * fxn;
        
        this.elements.set([
            rxn, ux, -fxn, 0,
            ryn, uy, -fyn, 0,
            rzn, uz, -fzn, 0,
            -(rxn * eyeX + ryn * eyeY + rzn * eyeZ),
            -(ux * eyeX + uy * eyeY + uz * eyeZ),
            fxn * eyeX + fyn * eyeY + fzn * eyeZ,
            1
        ]);
        
        return this;
    }
    
    perspective(fov: number, aspect: number, near: number, far: number): this {
        const f = 1.0 / Math.tan(fov / 2);
        const rangeInv = 1 / (near - far);
        
        this.elements.set([
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (near + far) * rangeInv, -1,
            0, 0, near * far * rangeInv * 2, 0
        ]);
        
        return this;
    }

    // 添加正交投影矩阵实现
    orthographic(
        left: number, 
        right: number, 
        top: number, 
        bottom: number, 
        near: number, 
        far: number
    ): this {
        const rl = right - left;
        const tb = top - bottom;
        const fn = far - near;
        
        this.elements.set([
            2 / rl, 0, 0, 0,
            0, 2 / tb, 0, 0,
            0, 0, -2 / fn, 0,
            -(right + left) / rl, -(top + bottom) / tb, -(far + near) / fn, 1
        ]);
        
        return this;
    }
    
}