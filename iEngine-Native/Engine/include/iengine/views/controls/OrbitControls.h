#pragma once

#include "BaseControls.h"
#include "../cameras/PerspectiveCamera.h"
#include "../../math/Vector3.h"
#include <memory>

namespace iengine {
    class OrbitControls : public BaseControls {
    public:
        OrbitControls(std::shared_ptr<PerspectiveCamera> camera, void* domElement);
        ~OrbitControls();
        
        void dispose() override;
        
    private:
        std::shared_ptr<PerspectiveCamera> camera_;
        void* domElement_;
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