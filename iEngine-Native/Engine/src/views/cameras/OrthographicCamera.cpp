#include "../../../include/iengine/views/cameras/OrthographicCamera.h"

#include <algorithm>

namespace iengine {

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float near, float far)
        : left_(left), right_(right), bottom_(bottom), top_(top), near_(near), far_(far) {
        updateProjectionMatrix();
    }

    OrthographicCamera::~OrthographicCamera() {}

    float OrthographicCamera::getLeft() const {
        return left_;
    }

    float OrthographicCamera::getRight() const {
        return right_;
    }

    float OrthographicCamera::getBottom() const {
        return bottom_;
    }

    float OrthographicCamera::getTop() const {
        return top_;
    }

    float OrthographicCamera::getNear() const {
        return near_;
    }

    float OrthographicCamera::getFar() const {
        return far_;
    }

    void OrthographicCamera::setLeft(float left) {
        left_ = left;
        projectionMatrixDirty_ = true;
    }

    void OrthographicCamera::setRight(float right) {
        right_ = right;
        projectionMatrixDirty_ = true;
    }

    void OrthographicCamera::setBottom(float bottom) {
        bottom_ = bottom;
        projectionMatrixDirty_ = true;
    }

    void OrthographicCamera::setTop(float top) {
        top_ = top;
        projectionMatrixDirty_ = true;
    }

    void OrthographicCamera::setNear(float near) {
        near_ = near;
        projectionMatrixDirty_ = true;
    }

    void OrthographicCamera::setFar(float far) {
        far_ = far;
        projectionMatrixDirty_ = true;
    }

    void OrthographicCamera::updateProjectionMatrix() {
        if (!projectionMatrixDirty_) {
            return;
        }

        // 创建正交投影矩阵
        projectionMatrix_.orthographic(left_, right_, top_, bottom_, near_, far_);

        projectionMatrixDirty_ = false;
    }

} // namespace iengine