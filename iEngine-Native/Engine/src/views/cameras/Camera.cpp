#include "iengine/views/cameras/Camera.h"

namespace iengine {
    Camera::Camera() {
        updateViewMatrix();
    }
    
    void Camera::setPosition(float x, float y, float z) {
        position_.set(x, y, z);
        viewMatrixDirty_ = true;
    }
    
    void Camera::setPosition(const Vector3& position) {
        position_ = position;
        viewMatrixDirty_ = true;
    }
    
    void Camera::setTarget(float x, float y, float z) {
        target_.set(x, y, z);
        viewMatrixDirty_ = true;
    }
    
    void Camera::setTarget(const Vector3& target) {
        target_ = target;
        viewMatrixDirty_ = true;
    }
    
    void Camera::setUp(float x, float y, float z) {
        up_.set(x, y, z);
        viewMatrixDirty_ = true;
    }
    
    void Camera::setUp(const Vector3& up) {
        up_ = up;
        viewMatrixDirty_ = true;
    }
    
    void Camera::updateViewMatrix() {
        if (!viewMatrixDirty_) return;
        
        viewMatrix_.lookAt(
            position_.x, position_.y, position_.z,
            target_.x, target_.y, target_.z,
            up_.x, up_.y, up_.z
        );
        
        viewMatrixDirty_ = false;
    }
    
    const Matrix4& Camera::getViewMatrix() {
        updateViewMatrix();
        return viewMatrix_;
    }
    
    Matrix4 Camera::getViewProjectionMatrix() {
        updateViewMatrix();
        updateProjectionMatrix();
        Matrix4 result = projectionMatrix_;
        result.multiply(viewMatrix_);
        return result;
    }
}