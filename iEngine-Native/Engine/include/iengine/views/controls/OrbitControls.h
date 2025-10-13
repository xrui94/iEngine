#pragma once

#include "BaseControls.h"
#include "../cameras/PerspectiveCamera.h"
#include "../../math/Vector3.h"
#include "../../windowing/Window.h"
#include <memory>

namespace iengine {
    class WindowInterface; // 前向声明
    
    class OrbitControls : public BaseControls, public WindowEventListener {
    public:
        OrbitControls(std::shared_ptr<PerspectiveCamera> camera, std::shared_ptr<WindowInterface> window);
        ~OrbitControls() override;
        
        void dispose() override;
        
        // WindowEventListener 接口实现
        bool onWindowEvent(const WindowEvent& event) override;
        int getPriority() const override { return 100; } // 控制器优先级
        
        // 向后兼容的事件处理方法
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