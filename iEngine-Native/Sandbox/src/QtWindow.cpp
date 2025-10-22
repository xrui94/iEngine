#include "QtWindow.h"
#include <glad/glad.h>  // GLAD必须在所有OpenGL相关头文件之前包含
#include <iengine/iengine.h>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
// 注意：glad必须在Qt的OpenGL头文件之前包含，以避免冲突
#include <iostream>

namespace sandbox {

    QtWindow::QtWindow(iengine::Engine& engine, QWidget* parent)
        : QOpenGLWidget(parent)
        , engine_(engine)   // 绑定引用
        , eventCallback_(nullptr)
        , eventDispatcher_(std::make_unique<iengine::WindowEventDispatcher>())
        , shouldClose_(false)
        , initialized_(false)
        , renderTimer_(new QTimer(this))
    {
        // 设置 OpenGL 格式（必须在 widget 创建后）
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        setFormat(format);
        
        // 设置焦点，以便接收键盘事件
        setFocusPolicy(Qt::StrongFocus);
        
        // 设置连续渲染定时器（60FPS）
        connect(renderTimer_, &QTimer::timeout, this, [this]() {
            this->update(); // 触发 paintGL
        });
        renderTimer_->setInterval(16); // 约60FPS (1000ms/60 ≈ 16ms)
        renderTimer_->setTimerType(Qt::PreciseTimer); // 使用精确定时器提高帧率稳定性
        
        std::cout << "QtWindow: 构造函数完成，使用观察者模式事件系统和连续渲染" << std::endl;
    }

    QtWindow::~QtWindow() {
        std::cout << "QtWindow: 开始析构" << std::endl;
        
        // 安全地清理事件监听器
        if (eventDispatcher_) {
            eventDispatcher_->clearEventListeners();
            std::cout << "QtWindow: 已清理所有事件监听器" << std::endl;
        }
        
        //// 清理引擎和场景引用（但不释放它们）
        //engine_.reset();
        //std::cout << "QtWindow: 已清理引擎和场景引用" << std::endl;
        //
        //// 最后清理 Context（只有在没有其他持有者时才会真正释放）
        //if (context_) {
        //    std::cout << "QtWindow: 准备释放 Context，use_count=" << context_.use_count() << std::endl;
        //    context_.reset();
        //    std::cout << "QtWindow: Context 引用已清理" << std::endl;
        //}
        
        std::cout << "QtWindow: 析构完成" << std::endl;
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

    std::shared_ptr<iengine::Context> QtWindow::getContext() const
    {
		return engine_.getActiveScene()->getContext();
    }

    void QtWindow::makeContextCurrent() {
        makeCurrent();
    }

    void QtWindow::setEventCallback(const iengine::WindowEventCallback& callback) {
        eventCallback_ = callback;
    }
    
    iengine::WindowEventDispatcher& QtWindow::getEventDispatcher() {
        return *eventDispatcher_;
    }

    const iengine::WindowEventDispatcher& QtWindow::getEventDispatcher() const {
        return *eventDispatcher_;
    }
    
    void QtWindow::startContinuousRendering() {
        if (!renderTimer_->isActive()) {
            renderTimer_->start();
            std::cout << "QtWindow: 连续渲染已启动 (精确60FPS)" << std::endl;
        }
    }
    
    void QtWindow::stopContinuousRendering() {
        if (renderTimer_->isActive()) {
            renderTimer_->stop();
            std::cout << "QtWindow: 连续渲染已停止，已启用事件驱动智能重绘" << std::endl;
        }
    }
    
    bool QtWindow::isContinuousRendering() const {
        return renderTimer_->isActive();
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

        engine_.start();
        
        // 默认启动连续渲染定时器（优化后的设置）
        renderTimer_->start();
        std::cout << "QtWindow: 连续渲染定时器已启动 (精确60FPS)" << std::endl;
    }

    void QtWindow::resizeGL(int w, int h) {
        glViewport(0, 0, w, h);
        std::cout << "Qt窗口大小调整为: " << w << "x" << h << std::endl;
        
        // 分发resize事件
        iengine::WindowEvent event;
        event.type = iengine::WindowEventType::Resize;
        event.data.resize.width = w;
        event.data.resize.height = h;
        eventDispatcher_->dispatchEvent(event);
        
        // 【旧版本已弃用】通过回调函数分发（会导致双重处理）
        // if (eventCallback_) {
        //     eventCallback_(event);
        // }
    }

    void QtWindow::paintGL() {
        // 清除屏幕
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用Engine渲染场景
        if (engine_.isReady() && initialized_) {
            try {
                //engine_->render();
                engine_.tick();
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

    // Qt输入事件处理
    void QtWindow::mousePressEvent(QMouseEvent* event) {
        iengine::WindowEvent windowEvent;
        windowEvent.type = iengine::WindowEventType::MouseButton;
        windowEvent.data.mouseButton.button = qtButtonToEngineButton(event->button());
        windowEvent.data.mouseButton.action = iengine::KeyAction::Press;
        windowEvent.data.mouseButton.x = event->x();
        windowEvent.data.mouseButton.y = event->y();
        
        bool eventHandled = eventDispatcher_->dispatchEvent(windowEvent);
        
        // 在事件驱动模式下，如果事件被处理了，就触发重绘
        if (eventHandled && !renderTimer_->isActive()) {
            update(); // 事件驱动模式下的智能重绘
        }
        
        if (!eventHandled) {
            QOpenGLWidget::mousePressEvent(event);
        }
    }

    void QtWindow::mouseReleaseEvent(QMouseEvent* event) {
        iengine::WindowEvent windowEvent;
        windowEvent.type = iengine::WindowEventType::MouseButton;
        windowEvent.data.mouseButton.button = qtButtonToEngineButton(event->button());
        windowEvent.data.mouseButton.action = iengine::KeyAction::Release;
        windowEvent.data.mouseButton.x = event->x();
        windowEvent.data.mouseButton.y = event->y();
        
        bool eventHandled = eventDispatcher_->dispatchEvent(windowEvent);
        
        // 在事件驱动模式下，如果事件被处理了，就触发重绘
        if (eventHandled && !renderTimer_->isActive()) {
            update(); // 事件驱动模式下的智能重绘
        }
        
        if (!eventHandled) {
            QOpenGLWidget::mouseReleaseEvent(event);
        }
    }

    void QtWindow::mouseMoveEvent(QMouseEvent* event) {
        iengine::WindowEvent windowEvent;
        windowEvent.type = iengine::WindowEventType::MouseMove;
        windowEvent.data.mouseMove.x = event->x();
        windowEvent.data.mouseMove.y = event->y();
        
        bool eventHandled = eventDispatcher_->dispatchEvent(windowEvent);
        
        // 在事件驱动模式下，如果事件被处理了，就触发重绘
        if (eventHandled && !renderTimer_->isActive()) {
            update(); // 事件驱动模式下的智能重绘（鼠标移动很关键）
        }
        
        if (!eventHandled) {
            QOpenGLWidget::mouseMoveEvent(event);
        }
    }

    void QtWindow::wheelEvent(QWheelEvent* event) {
        iengine::WindowEvent windowEvent;
        windowEvent.type = iengine::WindowEventType::MouseScroll;
        windowEvent.data.mouseScroll.xoffset = event->angleDelta().x() / 120.0;
        windowEvent.data.mouseScroll.yoffset = event->angleDelta().y() / 120.0;
        
        bool eventHandled = eventDispatcher_->dispatchEvent(windowEvent);
        
        // 在事件驱动模式下，如果事件被处理了，就触发重绘
        if (eventHandled && !renderTimer_->isActive()) {
            update(); // 事件驱动模式下的智能重绘（滚轮缩放很关键）
        }
        
        if (!eventHandled) {
            QOpenGLWidget::wheelEvent(event);
        }
    }

    void QtWindow::keyPressEvent(QKeyEvent* event) {
        iengine::WindowEvent windowEvent;
        windowEvent.type = iengine::WindowEventType::Key;
        windowEvent.data.key.key = qtKeyToEngineKey(event->key());
        windowEvent.data.key.action = iengine::KeyAction::Press;
        windowEvent.data.key.mods = 0;
        
        bool eventHandled = eventDispatcher_->dispatchEvent(windowEvent);
        
        // 在事件驱动模式下，如果事件被处理了，就触发重绘
        if (eventHandled && !renderTimer_->isActive()) {
            update(); // 事件驱动模式下的智能重绘（按键切换控制器等）
        }
        
        if (!eventHandled) {
            QOpenGLWidget::keyPressEvent(event);
        }
    }

    void QtWindow::keyReleaseEvent(QKeyEvent* event) {
        iengine::WindowEvent windowEvent;
        windowEvent.type = iengine::WindowEventType::Key;
        windowEvent.data.key.key = qtKeyToEngineKey(event->key());
        windowEvent.data.key.action = iengine::KeyAction::Release;
        windowEvent.data.key.mods = 0;
        
        bool eventHandled = eventDispatcher_->dispatchEvent(windowEvent);
        
        // 在事件驱动模式下，如果事件被处理了，就触发重绘
        if (eventHandled && !renderTimer_->isActive()) {
            update(); // 事件驱动模式下的智能重绘
        }
        
        if (!eventHandled) {
            QOpenGLWidget::keyReleaseEvent(event);
        }
    }
    
    // Qt事件转换辅助函数
    iengine::MouseButton QtWindow::qtButtonToEngineButton(Qt::MouseButton button) {
        switch (button) {
            case Qt::LeftButton: return iengine::MouseButton::Left;
            case Qt::RightButton: return iengine::MouseButton::Right;
            case Qt::MiddleButton: return iengine::MouseButton::Middle;
            default: return iengine::MouseButton::Left;
        }
    }

    iengine::KeyAction QtWindow::qtActionToEngineAction(bool pressed) {
        return pressed ? iengine::KeyAction::Press : iengine::KeyAction::Release;
    }

    int QtWindow::qtKeyToEngineKey(int qtKey) {
        // 简单映射，可以根据需要扩展
        switch (qtKey) {
            case Qt::Key_W: return 87; // 'W'
            case Qt::Key_A: return 65; // 'A'
            case Qt::Key_S: return 83; // 'S'
            case Qt::Key_D: return 68; // 'D'
            case Qt::Key_P: return 80; // 'P'
            case Qt::Key_Escape: return 256; // ESC
            default: return qtKey;
        }
    }

} // namespace sandbox