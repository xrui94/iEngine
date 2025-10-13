#pragma once

#include "BaseControls.h"
#include "../cameras/PerspectiveCamera.h"
#include "../../math/Vector3.h"
#include "../../windowing/Window.h"
#include <memory>

namespace iengine {
    class WindowInterface; // 前向声明
    
    class OrbitControls : public BaseControls {
    public:
        OrbitControls(std::shared_ptr<PerspectiveCamera> camera, std::shared_ptr<WindowInterface> window);
        ~OrbitControls();
        
        void dispose() override;
        
        // 事件处理方法（公开，供事件回调调用）
        void handleWindowEvent(const WindowEvent& event);
        
    private:
        std::shared_ptr<PerspectiveCamera> camera_;
        std::shared_ptr<WindowInterface> window_;
        Vector3 target_ = Vector3(0, 0, 0);
        bool isDragging_ = false;
        float lastX_ = 0;
        float lastY_ = 0;
        float theta_ = 0;
        float phi_ = 0;
        float radius_ = 5;
        
        void onMouseDown(float x, float y);
        void onMouseMove(float x, float y);
        void onMouseUp();
        void onMouseWheel(float delta);
        void updateCamera();
    };
}