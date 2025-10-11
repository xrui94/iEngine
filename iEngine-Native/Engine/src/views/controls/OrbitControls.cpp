#include "../../../include/iengine/views/controls/OrbitControls.h"
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cmath>

namespace iengine {
    OrbitControls::OrbitControls(std::shared_ptr<PerspectiveCamera> camera, void* domElement)
        : camera_(camera), domElement_(domElement) {
        
        // 初始化半径、theta和phi
        // 这里需要通过相机的公共接口获取位置信息
        
        // 保证初始状态
        updateCamera();
    }
    
    OrbitControls::~OrbitControls() {
        dispose();
    }
    
    void OrbitControls::dispose() {
        // 移除事件监听
    }
    
    void OrbitControls::onMouseDown(float x, float y) {
        isDragging_ = true;
        lastX_ = x;
        lastY_ = y;
    }
    
    void OrbitControls::onMouseMove(float x, float y) {
        if (!isDragging_) return;
        const float dx = x - lastX_;
        const float dy = y - lastY_;
        lastX_ = x;
        lastY_ = y;
        
        theta_ -= dx * 0.01f;
        phi_ -= dy * 0.01f;
        phi_ = std::max(0.01f, std::min(static_cast<float>(M_PI) - 0.01f, phi_));
        updateCamera();
    }
    
    void OrbitControls::onMouseUp() {
        isDragging_ = false;
    }
    
    void OrbitControls::onMouseWheel(float delta) {
        radius_ += delta * 0.01f;
        radius_ = std::max(1.0f, radius_);
        updateCamera();
    }
    
    void OrbitControls::updateCamera() {
        const float x = target_.x + radius_ * std::sin(phi_) * std::sin(theta_);
        const float y = target_.y + radius_ * std::cos(phi_);
        const float z = target_.z + radius_ * std::sin(phi_) * std::cos(theta_);
        camera_->setPosition(x, y, z);
        camera_->lookAt(target_.x, target_.y, target_.z);
    }
}