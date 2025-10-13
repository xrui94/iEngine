#include "QtWindow.h"
#include <glad/glad.h>  // GLAD必须在所有OpenGL相关头文件之前包含
#include <iengine/iengine.h>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QSurfaceFormat>
// 注意：glad必须在Qt的OpenGL头文件之前包含，以避免冲突
#include <iostream>

namespace sandbox {

    //// 内部Qt Widget类，继承QOpenGLWidget
    //class QtOpenGLWidget : public QOpenGLWidget {
    //public:
    //    QtOpenGLWidget(QtWindow* qtWindow, QWidget* parent = nullptr)
    //        : QOpenGLWidget(parent), qtWindow_(qtWindow) {
    //        
    //        // 设置OpenGL格式
    //        QSurfaceFormat format;
    //        format.setDepthBufferSize(24);
    //        format.setStencilBufferSize(8);
    //        format.setVersion(3, 3);
    //        format.setProfile(QSurfaceFormat::CoreProfile);
    //        setFormat(format);
    //    }

    //protected:
    //    void initializeGL() override {
    //        if (qtWindow_) {
    //            qtWindow_->initializeGL();
    //        }
    //    }

    //    void resizeGL(int w, int h) override {
    //        if (qtWindow_) {
    //            qtWindow_->resizeGL(w, h);
    //        }
    //    }

    //    void paintGL() override {
    //        if (qtWindow_) {
    //            qtWindow_->paintGL();
    //        }
    //    }

    //private:
    //    QtWindow* qtWindow_;
    //};

    QtWindow::QtWindow(QWidget* parent)
        : QOpenGLWidget(parent)
        , context_(nullptr)
        , eventCallback_(nullptr)
        , engine_(nullptr)
        , scene_(nullptr)
        , shouldClose_(false)
        , initialized_(false)
    {
        // 设置 OpenGL 格式（必须在 widget 创建后）
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        setFormat(format);
    }

    QtWindow::~QtWindow() {
        //if (widget_) {
        //    delete widget_;
        //    widget_ = nullptr;
        //}
    }

    //bool QtWindow::initialize(QWidget* parent) {
    //    //// 创建内部Qt OpenGL Widget
    //    //widget_ = new QtOpenGLWidget(this, parent);
    //    //if (!widget_) {
    //    //    std::cerr << "Failed to create Qt OpenGL Widget" << std::endl;
    //    //    return false;
    //    //}



    //    std::cout << "Qt窗口创建成功" << std::endl;
    //    return true;
    //}

    void QtWindow::getSize(int& width, int& height) const {
        width = this->width();
        height = this->height();
    }

    bool QtWindow::shouldClose() const {
        return shouldClose_;
    }

    std::shared_ptr<iengine::Context> QtWindow::getContext() const {
        return context_;
    }

    void QtWindow::makeContextCurrent() {
        makeCurrent();
    }

    void QtWindow::setEventCallback(const iengine::WindowEventCallback& callback) {
        eventCallback_ = callback;
    }

    void QtWindow::setEngine(std::shared_ptr<iengine::Engine> engine) {
        engine_ = engine;
    }

    void QtWindow::setScene(std::shared_ptr<iengine::Scene> scene) {
        scene_ = scene;
    }

    //void QtWindow::render() {
    //    if (widget_) {
    //        widget_->update(); // 触发paintGL
    //    }
    //}

    void QtWindow::initializeGL() {
        // 在Qt环境中也必须使用glad初始化OpenGL函数，保持与Engine一致
        //if (!gladLoadGL()) {
        //    std::cerr << "Failed to initialize GLAD in Qt window" << std::endl;
        //    return;
        //}
        //
        //std::cout << "Qt OpenGL初始化成功" << std::endl;
        //std::cout << "OpenGL版本: " << glGetString(GL_VERSION) << std::endl;
        //std::cout << "OpenGL厂商: " << glGetString(GL_VENDOR) << std::endl;
        //std::cout << "OpenGL渲染器: " << glGetString(GL_RENDERER) << std::endl;
        //
        //// 设置OpenGL状态（使用glad加载的函数）
        //glEnable(GL_DEPTH_TEST);
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // 使用 QOpenGLContext::getProcAddress 作为 GLAD 的 loader
        if (!gladLoadGLLoader([](const char* name) -> void* {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            if (!ctx) return nullptr;
            return reinterpret_cast<void*>(ctx->getProcAddress(name));
            })) {
            qFatal("Failed to initialize GLAD");
            return;
        }

        initialized_ = true;

        engine_->start();
    }

    void QtWindow::resizeGL(int w, int h) {
        glViewport(0, 0, w, h);
        std::cout << "Qt窗口大小调整为: " << w << "x" << h << std::endl;
    }

    void QtWindow::paintGL() {
        // 清除屏幕
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用Engine渲染场景
        if (engine_ && scene_ && initialized_) {
            try {
                //engine_->render();
                engine_->tick();
            } catch (const std::exception& e) {
                std::cerr << "Qt窗口Engine渲染失败: " << e.what() << std::endl;
                // 备用渲染：简单清屏
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
        } else {
            // 如果引擎或场景未初始化，显示默认背景
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }

} // namespace sandbox