#pragma once

#include "BaseControls.h"
#include "../cameras/PerspectiveCamera.h"
#include <memory>

namespace iengine {
    class FirstPersonControls : public BaseControls {
    public:
        FirstPersonControls(std::shared_ptr<PerspectiveCamera> camera, void* domElement);
        ~FirstPersonControls();
        
        void dispose() override;
        
    private:
        std::shared_ptr<PerspectiveCamera> camera_;
        void* domElement_;
        bool isDragging_ = false;
        float lastX_ = 0;
        float lastY_ = 0;
        float yaw_ = 0;
        float pitch_ = 0;
        float moveSpeed_ = 0.1f;
        float lookSpeed_ = 0.002f;
        
        void onMouseDown(float x, float y);
        void onMouseMove(float x, float y);
        void onMouseUp();
        void onMouseWheel(float delta);
        void onKeyDown(char key);
        void updateCamera();
    };
}