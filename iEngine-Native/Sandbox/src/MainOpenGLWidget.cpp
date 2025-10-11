#include <sandbox/MainOpenGLWidget.h>
#include <iengine/iengine.h>

#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <iostream>

namespace sandbox {

    MainOpenGLWidget::MainOpenGLWidget(QWidget* parent)
        : QOpenGLWidget(parent)
        , engine_(nullptr)
        , scene_(nullptr)
        , initialized_(false) {
        
        // 设置 OpenGL 格式
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        setFormat(format);
    }
    
    MainOpenGLWidget::~MainOpenGLWidget() {
        // 清理资源
    }
    
    void MainOpenGLWidget::setEngine(std::shared_ptr<iengine::Engine> engine) {
        engine_ = engine;
    }
    
    void MainOpenGLWidget::setScene(std::shared_ptr<iengine::Scene> scene) {
        scene_ = scene;
    }
    
    void MainOpenGLWidget::initializeGL() {
        // 初始化 OpenGL 函数
        initializeOpenGLFunctions();
        
        // 初始化引擎 OpenGL 上下文
        if (engine_) {
            try {
                // 这里应该初始化引擎的 OpenGL 上下文
                // 具体实现取决于 iengine 的设计
                initialized_ = true;
            } catch (const std::exception& e) {
                std::cerr << "Failed to initialize OpenGL: " << e.what() << std::endl;
            }
        }
    }
    
    void MainOpenGLWidget::resizeGL(int w, int h) {
        // 调整视口大小
        glViewport(0, 0, w, h);
        
        // 通知引擎调整大小
        if (engine_) {
            try {
                // 这里应该调用引擎的 resize 方法
                // 具体实现取决于 iengine 的设计
            } catch (const std::exception& e) {
                std::cerr << "Failed to resize OpenGL: " << e.what() << std::endl;
            }
        }
    }
    
    void MainOpenGLWidget::paintGL() {
        // 清除屏幕
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 渲染场景
        if (engine_ && scene_ && initialized_) {
            try {
                engine_->render(); // 现在这是公共方法
            } catch (const std::exception& e) {
                std::cerr << "Failed to render: " << e.what() << std::endl;
            }
        }
    }

} // namespace sandbox