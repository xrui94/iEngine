#include "iengine/views/controls/FirstPersonControls.h"

#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace iengine {
    FirstPersonControls::FirstPersonControls(std::shared_ptr<PerspectiveCamera> camera, void* domElement)
        : camera_(camera), domElement_(domElement) {
        
        // 初始化 yaw/pitch 与当前相机方向同步
        // 这里需要通过相机的公共接口获取方向信息
        
        // 保证初始状态
        updateCamera();
    }
    
    FirstPersonControls::~FirstPersonControls() {
        dispose();
    }
    
    void FirstPersonControls::dispose() {
        // 移除事件监听
    }
    
    void FirstPersonControls::onMouseDown(float x, float y) {
        isDragging_ = true;
        lastX_ = x;
        lastY_ = y;
    }
    
    void FirstPersonControls::onMouseMove(float x, float y) {
        if (!isDragging_) return;
        const float dx = x - lastX_;
        const float dy = y - lastY_;
        lastX_ = x;
        lastY_ = y;
        
        yaw_ -= dx * lookSpeed_;
        pitch_ -= dy * lookSpeed_;
        pitch_ = std::max(-static_cast<float>(M_PI) / 2, std::min(static_cast<float>(M_PI) / 2, pitch_));
        updateCamera();
    }
    
    void FirstPersonControls::onMouseUp() {
        isDragging_ = false;
    }
    
    void FirstPersonControls::onMouseWheel(float delta) {
        // 可用于前后移动
        // camera_->position_.z += delta * 0.01f;
    }
    
    void FirstPersonControls::onKeyDown(char key) {
        // 简单的WASD移动
        const float forwardX = std::sin(yaw_);
        const float forwardZ = std::cos(yaw_);
        const float rightX = std::cos(yaw_);
        const float rightZ = -std::sin(yaw_);
        
        if (key == 'w') {
            // camera_->position_.x += forwardX * moveSpeed_;
            // camera_->position_.z += forwardZ * moveSpeed_;
        }
        if (key == 's') {
            // camera_->position_.x -= forwardX * moveSpeed_;
            // camera_->position_.z -= forwardZ * moveSpeed_;
        }
        if (key == 'a') {
            // camera_->position_.x -= rightX * moveSpeed_;
            // camera_->position_.z -= rightZ * moveSpeed_;
        }
        if (key == 'd') {
            // camera_->position_.x += rightX * moveSpeed_;
            // camera_->position_.z += rightZ * moveSpeed_;
        }
        updateCamera();
    }
    
    void FirstPersonControls::updateCamera() {
        // 计算朝向
        const float dirX = std::sin(yaw_) * std::cos(pitch_);
        const float dirY = std::sin(pitch_);
        const float dirZ = std::cos(yaw_) * std::cos(pitch_);
        
        // const float lookAtX = camera_->position_.x + dirX;
        // const float lookAtY = camera_->position_.y + dirY;
        // const float lookAtZ = camera_->position_.z + dirZ;
        
        // camera_->lookAt(lookAtX, lookAtY, lookAtZ);
    }
}