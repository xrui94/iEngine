#pragma once

#include <memory>
#include "../../math/Matrix4.h"
#include "../../math/Vector3.h"

namespace iengine {
    class Camera {
    public:
        Camera();
        virtual ~Camera() = default;
        
        // 设置相机位置
        void setPosition(float x, float y, float z);
        void setPosition(const Vector3& position);
        
        // 设置相机目标
        void setTarget(float x, float y, float z);
        void setTarget(const Vector3& target);
        
        // 设置上方向
        void setUp(float x, float y, float z);
        void setUp(const Vector3& up);
        
        // 获取视图矩阵
        const Matrix4& getViewMatrix();
        
        // 获取投影矩阵
        virtual const Matrix4& getProjectionMatrix() = 0;
        
        // 获取视图投影矩阵
        Matrix4 getViewProjectionMatrix();
        
    protected:
        virtual void updateProjectionMatrix() = 0;
        void updateViewMatrix();
        
        Vector3 position_ = Vector3(0.0f, 0.0f, 5.0f);
        Vector3 target_ = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 up_ = Vector3(0.0f, 1.0f, 0.0f);
        
        Matrix4 projectionMatrix_;
        Matrix4 viewMatrix_;
        
        bool viewMatrixDirty_ = true;
        bool projectionMatrixDirty_ = true;
    };
}