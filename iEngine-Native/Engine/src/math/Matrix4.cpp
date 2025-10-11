#include "../../include/iengine/math/Matrix4.h"
#include "../../include/iengine/math/Vector3.h"
#include <cmath>
#include <algorithm>

namespace iengine {
    Matrix4::Matrix4() {
        // 单位矩阵
        elements = {{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }};
    }
    
    Matrix4::Matrix4(const std::array<float, 16>& elements) : elements(elements) {}
    
    Matrix4 Matrix4::identity() {
        return Matrix4();
    }
    
    Matrix4 Matrix4::fromTranslation(const Vector3& offset) {
        std::array<float, 16> elements = {{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            offset.x, offset.y, offset.z, 1
        }};
        return Matrix4(elements);
    }
    
    Matrix4 Matrix4::fromScaling(const Vector3& scale) {
        std::array<float, 16> elements = {{
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1
        }};
        return Matrix4(elements);
    }
    
    Matrix4 Matrix4::fromRotation(const Vector3& axis, float angleRad) {
        float x = axis.x, y = axis.y, z = axis.z;
        float len = std::sqrt(x * x + y * y + z * z);
        if (len == 0) return identity();
        float nx = x / len, ny = y / len, nz = z / len;
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        float t = 1 - c;
        
        std::array<float, 16> elements = {{
            t * nx * nx + c,     t * nx * ny - s * nz, t * nx * nz + s * ny, 0,
            t * nx * ny + s * nz, t * ny * ny + c,     t * ny * nz - s * nx, 0,
            t * nx * nz - s * ny, t * ny * nz + s * nx, t * nz * nz + c,     0,
            0, 0, 0, 1
        }};
        return Matrix4(elements);
    }
    
    Matrix4& Matrix4::setIdentity() {
        elements = {{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }};
        return *this;
    }
    
    Matrix4& Matrix4::multiply(const Matrix4& other) {
        const auto& a = elements;
        const auto& b = other.elements;
        std::array<float, 16> result;
        
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result[j * 4 + i] = 0;
                for (int k = 0; k < 4; k++) {
                    result[j * 4 + i] += a[k * 4 + i] * b[j * 4 + k];
                }
            }
        }
        
        elements = result;
        return *this;
    }
    
    Matrix4& Matrix4::inverse() {
        const auto& m = elements;
        std::array<float, 16> inv;

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

        float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (det == 0) {
            // 如果行列式为0，返回单位矩阵
            return setIdentity();
        }

        det = 1.0f / det;
        for (int i = 0; i < 16; i++) {
            inv[i] *= det;
        }
        elements = inv;
        return *this;
    }

    Matrix4& Matrix4::transpose() {
        const auto& m = elements;
        std::array<float, 16> t;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                t[i * 4 + j] = m[j * 4 + i];
            }
        }
        elements = t;
        return *this;
    }

    Matrix4& Matrix4::lookAt(float eyeX, float eyeY, float eyeZ,
                            float targetX, float targetY, float targetZ,
                            float upX, float upY, float upZ) {
        
        // 计算前向向量
        float fx = targetX - eyeX;
        float fy = targetY - eyeY;
        float fz = targetZ - eyeZ;
        
        // 归一化前向向量
        float flen = std::sqrt(fx * fx + fy * fy + fz * fz);
        if (flen == 0) {
            // 如果目标点和眼睛重合，则默认朝-z
            fz = -1;
            flen = 1;
        }
        float fxn = fx / flen;
        float fyn = fy / flen;
        float fzn = fz / flen;
        
        // 计算右向量 (前向 x 上方向)
        float rx = fyn * upZ - fzn * upY;
        float ry = fzn * upX - fxn * upZ;
        float rz = fxn * upY - fyn * upX;
        
        // 归一化右向量
        float rlen = std::sqrt(rx * rx + ry * ry + rz * rz);
        if (rlen == 0) {
            // 如果 up 向量与前向共线，选择一个默认up
            if (std::abs(upZ) == 1) {
                rx = 1; ry = 0; rz = 0; rlen = 1;
            } else {
                rx = 0; ry = 0; rz = 1; rlen = 1;
            }
        }
        float rxn = rx / rlen;
        float ryn = ry / rlen;
        float rzn = rz / rlen;
        
        // 计算上向量 (右向量 x 前向)
        float ux = ryn * fzn - rzn * fyn;
        float uy = rzn * fxn - rxn * fzn;
        float uz = rxn * fyn - ryn * fxn;
        
        elements = {{
            rxn, ux, -fxn, 0,
            ryn, uy, -fyn, 0,
            rzn, uz, -fzn, 0,
            -(rxn * eyeX + ryn * eyeY + rzn * eyeZ),
            -(ux * eyeX + uy * eyeY + uz * eyeZ),
            fxn * eyeX + fyn * eyeY + fzn * eyeZ,
            1
        }};
        
        return *this;
    }
    
    Matrix4& Matrix4::perspective(float fov, float aspect, float near, float far) {
        float f = 1.0f / std::tan(fov / 2);
        float rangeInv = 1 / (near - far);
        
        elements = {{
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (near + far) * rangeInv, -1,
            0, 0, near * far * rangeInv * 2, 0
        }};
        
        return *this;
    }

    Matrix4& Matrix4::orthographic(float left, float right, float top, float bottom, float near, float far) {
        float rl = right - left;
        float tb = top - bottom;
        float fn = far - near;
        
        elements = {{
            2 / rl, 0, 0, 0,
            0, 2 / tb, 0, 0,
            0, 0, -2 / fn, 0,
            -(right + left) / rl, -(top + bottom) / tb, -(far + near) / fn, 1
        }};
        
        return *this;
    }
}