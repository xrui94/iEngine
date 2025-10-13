#pragma once

#include "BaseControls.h"
#include "../cameras/PerspectiveCamera.h"
#include "../../windowing/Window.h"
#include <memory>

namespace iengine {
    class WindowInterface; // 前向声明
    
    class FirstPersonControls : public BaseControls {
    public:
        FirstPersonControls(std::shared_ptr<PerspectiveCamera> camera, std::shared_ptr<WindowInterface> window);
        ~FirstPersonControls();
        
        void dispose() override;
        
        // 事件处理方法（公开，供事件回调调用）
        void handleWindowEvent(const WindowEvent& event);
        
    private:
        std::shared_ptr<PerspectiveCamera> camera_;
        std::shared_ptr<WindowInterface> window_;
        bool isDragging_ = false;
        float lastX_ = 0;
        float lastY_ = 0;
        float yaw_ = 0;
        float pitch_ = 0;
        float moveSpeed_ = 0.1f;
        float lookSpeed_ = 0.002f;
        
        // 维护本地相机位置状态（因为Camera::position_是protected）
        Vector3 currentPosition_ = Vector3(0.0f, 0.0f, 5.0f);
        
        void onMouseDown(float x, float y);
        void onMouseMove(float x, float y);
        void onMouseUp();
        void onMouseWheel(float delta);
        void onKeyDown(int key, int action);
        void updateCamera();
    };
}