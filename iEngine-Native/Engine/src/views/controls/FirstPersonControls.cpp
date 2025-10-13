#include "iengine/views/controls/FirstPersonControls.h"
#include "../../../include/iengine/views/controls/FirstPersonControls.h"
#include "iengine/windowing/Window.h"
#include <algorithm>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace iengine {
    FirstPersonControls::FirstPersonControls(std::shared_ptr<PerspectiveCamera> camera, std::shared_ptr<WindowInterface> window)
        : camera_(camera), window_(window) {
        
        // 初始化 yaw/pitch 让相机朝向原点 (0,0,0)
        // 从位置 (0,0,5) 朝向 (0,0,0)，需要 yaw=π（180度转身）
        yaw_ = static_cast<float>(M_PI);  // 180度，朝向 -Z 方向
        pitch_ = 0.0f;  // 水平朝向
        
        // 从相机获取当前位置，而不是硬编码
        // TODO: 由于position_是protected，暂时使用默认值，但需要与场景设置保持一致
        currentPosition_ = Vector3(0.0f, 0.0f, 5.0f);
        camera_->setPosition(currentPosition_.x, currentPosition_.y, currentPosition_.z);
        
        std::cout << "FirstPersonControls: 相机位置设置为 (" << currentPosition_.x 
                  << ", " << currentPosition_.y << ", " << currentPosition_.z << ")" << std::endl;
        
        // 使用lambda捕获this指针来绑定事件处理器
        window_->setEventCallback([this](const WindowEvent& event) {
            this->handleWindowEvent(event);
        });
        
        std::cout << "FirstPersonControls: 初始化完成，支持WASD移动和鼠标视角控制" << std::endl;
        
        // 保证初始状态
        updateCamera();
    }
    
    FirstPersonControls::~FirstPersonControls() {
        dispose();
    }
    
    void FirstPersonControls::dispose() {
        // 移除事件监听，重置窗口事件回调
        if (window_) {
            window_->setEventCallback(nullptr);
        }
    }
    
    bool FirstPersonControls::onWindowEvent(const WindowEvent& event) {
        // 直接调用现有的处理逻辑
        handleWindowEvent(event);
        
        // 返回false表示不阻断事件传播，允许其他监听器处理
        return false;
    }
    
    void FirstPersonControls::handleWindowEvent(const WindowEvent& event) {
        switch (event.type) {
            case WindowEventType::MouseButton:
                std::cout << "FirstPersonControls: 收到鼠标按键事件 - 按键:" << (int)event.data.mouseButton.button 
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
                    std::cout << "FirstPersonControls: 鼠标拖拽中 - 位置:(" << event.data.mouseMove.x << ", " << event.data.mouseMove.y << ")" << std::endl;
                }
                onMouseMove(static_cast<float>(event.data.mouseMove.x), 
                           static_cast<float>(event.data.mouseMove.y));
                break;
            case WindowEventType::MouseScroll: {
                std::cout << "FirstPersonControls: 收到滚轮事件 - 偏移:" << event.data.mouseScroll.yoffset << std::endl;
                // 防御性判断：限制滚轮值范围以防止异常值
                float wheelDelta = static_cast<float>(event.data.mouseScroll.yoffset);
                if (std::abs(wheelDelta) > 10.0f) {
                    wheelDelta = wheelDelta > 0 ? 10.0f : -10.0f;
                }
                // 用于前后移动
                onMouseWheel(wheelDelta);
                break;
            }
            case WindowEventType::Key:
                std::cout << "FirstPersonControls: 收到键盘事件 - 按键:" << event.data.key.key 
                         << ", 动作:" << (int)event.data.key.action << std::endl;
                onKeyDown(event.data.key.key, (int)event.data.key.action);
                break;
            default:
                // 其他事件不处理
                break;
        }
    }
    
    void FirstPersonControls::onMouseDown(float x, float y) {
        isDragging_ = true;
        lastX_ = x;
        lastY_ = y;
        std::cout << "FirstPersonControls: 开始拖拽 - 位置:(" << x << ", " << y << ")" << std::endl;
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
        std::cout << "FirstPersonControls: 停止拖拽" << std::endl;
    }
    
    void FirstPersonControls::onMouseWheel(float delta) {
        // 防御性判断：如果值过小则忽略，防止微小抖动
        if (std::abs(delta) < 0.001f) {
            return;
        }
        
        // 用于前后移动（参考TS版本）
        // 计算前方向
        const float forwardX = std::sin(yaw_);
        const float forwardZ = std::cos(yaw_);
        
        // 限制移动速度
        float moveAmount = delta * 0.01f;
        moveAmount = std::max(-1.0f, std::min(1.0f, moveAmount));
        
        // 获取当前相机位置并移动（直接使用setPosition方法）
        // 需要先获取当前位置，但由于position_是protected，我们维护本地位置状态
        currentPosition_.x += forwardX * moveAmount;
        currentPosition_.z += forwardZ * moveAmount;
        camera_->setPosition(currentPosition_.x, currentPosition_.y, currentPosition_.z);
        updateCamera();
    }
    
    void FirstPersonControls::onKeyDown(int key, int action) {
        // 只在按下时处理（不处理重复事件）
        if (action != (int)KeyAction::Press) {
            return;
        }
        
        // 简单的WASD移动（参考TS版本）
        const float forwardX = std::sin(yaw_);
        const float forwardZ = std::cos(yaw_);
        const float rightX = std::cos(yaw_);
        const float rightZ = -std::sin(yaw_);
        
        // GLFW键码对应（参考GLFW定义）
        const int GLFW_KEY_W = 87;
        const int GLFW_KEY_S = 83;
        const int GLFW_KEY_A = 65;
        const int GLFW_KEY_D = 68;
        
        if (key == GLFW_KEY_W) {
            std::cout << "FirstPersonControls: W键 - 向前移动" << std::endl;
            currentPosition_.x += forwardX * moveSpeed_;
            currentPosition_.z += forwardZ * moveSpeed_;
            camera_->setPosition(currentPosition_.x, currentPosition_.y, currentPosition_.z);
        }
        if (key == GLFW_KEY_S) {
            std::cout << "FirstPersonControls: S键 - 向后移动" << std::endl;
            currentPosition_.x -= forwardX * moveSpeed_;
            currentPosition_.z -= forwardZ * moveSpeed_;
            camera_->setPosition(currentPosition_.x, currentPosition_.y, currentPosition_.z);
        }
        if (key == GLFW_KEY_A) {
            std::cout << "FirstPersonControls: A键 - 向左移动" << std::endl;
            currentPosition_.x -= rightX * moveSpeed_;
            currentPosition_.z -= rightZ * moveSpeed_;
            camera_->setPosition(currentPosition_.x, currentPosition_.y, currentPosition_.z);
        }
        if (key == GLFW_KEY_D) {
            std::cout << "FirstPersonControls: D键 - 向右移动" << std::endl;
            currentPosition_.x += rightX * moveSpeed_;
            currentPosition_.z += rightZ * moveSpeed_;
            camera_->setPosition(currentPosition_.x, currentPosition_.y, currentPosition_.z);
        }
        updateCamera();
    }
    
    void FirstPersonControls::updateCamera() {
        // 计算朝向（参考TS版本）
        const float dirX = std::sin(yaw_) * std::cos(pitch_);
        const float dirY = std::sin(pitch_);
        const float dirZ = std::cos(yaw_) * std::cos(pitch_);
        
        // 计算目标点
        const float lookAtX = currentPosition_.x + dirX;
        const float lookAtY = currentPosition_.y + dirY;
        const float lookAtZ = currentPosition_.z + dirZ;
        
        // 设置相机朝向
        camera_->lookAt(lookAtX, lookAtY, lookAtZ);
    }
}