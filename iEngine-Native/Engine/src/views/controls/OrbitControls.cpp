#include "iengine/views/controls/OrbitControls.h"
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cmath>
#include <iostream>

namespace iengine {
    OrbitControls::OrbitControls(std::shared_ptr<PerspectiveCamera> camera, std::shared_ptr<WindowInterface> window)
        : camera_(camera), window_(window) {
        
        // 初始化半径、theta和phi，计算当前相机位置的球坐标
        // 注意：Native版本的Camera位置是私有成员，需要通过getter方法访问
        // 但是目前的接口不提供 getter，所以我们使用默认值
        float dx = 0.0f; // 默认相机位置 (0, 0, 5)
        float dy = 0.0f; 
        float dz = 5.0f;
        radius_ = std::sqrt(dx * dx + dy * dy + dz * dz);
        theta_ = std::atan2(dx, dz);
        phi_ = std::acos(dy / radius_);
        
        // 注册窗口事件回调，使用引擎的抽象接口
        if (window_) {
            window_->setEventCallback([this](const WindowEvent& event) {
                this->handleWindowEvent(event);
            });
            std::cout << "OrbitControls: 已注册窗口事件回调，支持鼠标交互" << std::endl;
        }
        
        std::cout << "OrbitControls: 初始化完成，半径=" << radius_ << ", theta=" << theta_ << ", phi=" << phi_ << std::endl;
        
        // 保证初始状态
        updateCamera();
    }
    
    OrbitControls::~OrbitControls() {
        dispose();
    }
    
    void OrbitControls::dispose() {
        // 移除事件监听，重置窗口事件回调
        if (window_) {
            window_->setEventCallback(nullptr);
        }
    }
    
    bool OrbitControls::onWindowEvent(const WindowEvent& event) {
        // 直接调用现有的处理逻辑
        handleWindowEvent(event);
        
        // 返回false表示不阻断事件传播，允许其他监听器处理
        // 如果需要阻断特定事件（比如鼠标拖拽），可以根据事件类型返回true
        return false;
    }
    
    void OrbitControls::handleWindowEvent(const WindowEvent& event) {
        switch (event.type) {
            case WindowEventType::MouseButton:
                std::cout << "OrbitControls: 收到鼠标按键事件 - 按键:" << (int)event.data.mouseButton.button 
                         << ", 动作:" << (int)event.data.mouseButton.action 
                         << ", 位置:(" << event.data.mouseButton.x << ", " << event.data.mouseButton.y << ")" << std::endl;
                if (event.data.mouseButton.button == MouseButton::Left) {
                    if (event.data.mouseButton.action == KeyAction::Press) {
                        onMouseDown(static_cast<float>(event.data.mouseButton.x), 
                                   static_cast<float>(event.data.mouseButton.y));
                    } else if (event.data.mouseButton.action == KeyAction::Release) {
                        onMouseUp();
                    }
                }
                break;
            case WindowEventType::MouseMove:
                if (isDragging_) {
                    std::cout << "OrbitControls: 鼠标拖拽中 - 位置:(" << event.data.mouseMove.x << ", " << event.data.mouseMove.y << ")" << std::endl;
                }
                onMouseMove(static_cast<float>(event.data.mouseMove.x), 
                           static_cast<float>(event.data.mouseMove.y));
                break;
            case WindowEventType::MouseScroll: {
                std::cout << "OrbitControls: 收到滚轮事件 - 偏移:" << event.data.mouseScroll.yoffset << std::endl;
                // 防御性判断：限制滚轮值范围以防止异常值
                float wheelDelta = static_cast<float>(event.data.mouseScroll.yoffset);
                if (std::abs(wheelDelta) > 10.0f) {
                    wheelDelta = wheelDelta > 0 ? 10.0f : -10.0f;
                }
                // GLFW与Web的滚轮方向相反，所以需要取反
                onMouseWheel(-wheelDelta);
                break;
            }
            default:
                // 其他事件不处理
                break;
        }
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
        // 防御性判断：如果值过小则忽略，防止微小抖动
        if (std::abs(delta) < 0.001f) {
            return;
        }
        
        float oldRadius = radius_;
        radius_ += delta * 0.01f;
        
        // 限制缩放范围：最小1.0，最大100.0
        radius_ = std::max(1.0f, std::min(100.0f, radius_));
        
        // 只有当半径真正改变时才更新相机
        if (std::abs(radius_ - oldRadius) > 0.001f) {
            updateCamera();
        }
    }
    
    void OrbitControls::updateCamera() {
        const float x = target_.x + radius_ * std::sin(phi_) * std::sin(theta_);
        const float y = target_.y + radius_ * std::cos(phi_);
        const float z = target_.z + radius_ * std::sin(phi_) * std::cos(theta_);
        camera_->setPosition(x, y, z);
        camera_->lookAt(target_.x, target_.y, target_.z);
    }
}