#pragma once

#include "Camera.h"

namespace iengine {
    class PerspectiveCamera : public Camera {
    public:
        float fov = 45.0f;      // 视野角度（度）
        float aspect = 1.0f;    // 宽高比
        float near = 0.1f;      // 近平面
        float far = 100.0f;     // 远平面
        
        PerspectiveCamera(float fov = 45.0f, float aspect = 1.0f, float near = 0.1f, float far = 100.0f);
        
        void lookAt(float x, float y, float z);
        
        // 设置视野角度
        void setFov(float fov);
        
        // 设置宽高比
        void setAspect(float aspect);
        
        // 设置近平面
        void setNear(float near);
        
        // 设置远平面
        void setFar(float far);
        
        // 实现投影矩阵更新
        void updateProjectionMatrix() override;
        
        // 实现获取投影矩阵的方法
        const Matrix4& getProjectionMatrix() override;
    };
}