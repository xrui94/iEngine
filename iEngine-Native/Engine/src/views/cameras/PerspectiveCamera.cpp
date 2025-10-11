#include "../../../include/iengine/views/cameras/PerspectiveCamera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace iengine {
    PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
        : fov(fov), aspect(aspect), near(near), far(far) {
        updateProjectionMatrix();
    }
    
    void PerspectiveCamera::lookAt(float x, float y, float z) {
        setTarget(x, y, z);
    }
    
    void PerspectiveCamera::setFov(float fov) {
        this->fov = fov;
        projectionMatrixDirty_ = true;
    }
    
    void PerspectiveCamera::setAspect(float aspect) {
        this->aspect = aspect;
        projectionMatrixDirty_ = true;
    }
    
    void PerspectiveCamera::setNear(float near) {
        this->near = near;
        projectionMatrixDirty_ = true;
    }
    
    void PerspectiveCamera::setFar(float far) {
        this->far = far;
        projectionMatrixDirty_ = true;
    }
    
    void PerspectiveCamera::updateProjectionMatrix() {
        if (!projectionMatrixDirty_) return;
        
        projectionMatrix_.perspective(
            fov * static_cast<float>(M_PI) / 180.0f, // 转换为弧度
            aspect,
            near,
            far
        );
        
        projectionMatrixDirty_ = false;
    }
    
    const Matrix4& PerspectiveCamera::getProjectionMatrix() {
        updateProjectionMatrix();
        return projectionMatrix_;
    }
}