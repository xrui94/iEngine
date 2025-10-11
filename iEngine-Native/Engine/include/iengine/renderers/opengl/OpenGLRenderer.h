#pragma once

#include "../Renderer.h"
#include <memory>

namespace iengine {
    // 前向声明
    class Camera;
    class Scene;
    class Mesh;
    class Material;
    class Context;
    
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        
        bool initialize() override;
        void cleanup() override;
        void render(std::shared_ptr<Scene> scene) override;
        void resize(int width, int height) override;
        void clear() override;
        
        void init(std::shared_ptr<Context> context);
        
    private:
        std::shared_ptr<Context> context_;
        std::shared_ptr<Camera> currentCamera_;
    };
}