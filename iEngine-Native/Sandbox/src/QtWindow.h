#pragma once

#include <glad/glad.h>  // GLAD必须在所有OpenGL相关头文件之前
#include <iengine/iengine.h>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTimer>
#include <memory>

// 前向声明
class QWidget;

namespace sandbox {

    /**
     * @brief Qt窗口实现，符合Engine的WindowInterface接口
     * 
     * 这个类将Qt的QOpenGLWidget封装为Engine可以使用的窗口抽象，
     * 实现了窗口大小查询、关闭状态检查、上下文管理等功能。
     */
	class QtWindow : public QOpenGLWidget, public iengine::WindowInterface {    // 第一个继承的类必须是QOpenGLWidget，QT中多继承时，第一个基类必须是Qt中的类
        Q_OBJECT
    public:
        QtWindow(iengine::Engine& engine, QWidget* parent = nullptr);
        ~QtWindow() override;

        // WindowInterface 接口实现
        void getSize(int& width, int& height) const override;
        bool shouldClose() const override;
        std::shared_ptr<iengine::Context> getContext() const override;
        void makeContextCurrent() override;
        void setEventCallback(const iengine::WindowEventCallback& callback) override;
        
        // 观察者模式事件接口实现
        iengine::WindowEventDispatcher& getEventDispatcher() override;
        const iengine::WindowEventDispatcher& getEventDispatcher() const override;

        // Qt特有的方法
        void setEngine(std::shared_ptr<iengine::Engine> engine);
        
        // 连续渲染控制
        void startContinuousRendering();
        void stopContinuousRendering();
        bool isContinuousRendering() const;

    protected:
        // 内部初始化方法 - 设为公有以便内部Widget调用
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;
        
        // Qt输入事件处理
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;

    private:
        // 转换Qt事件到引擎事件
        iengine::MouseButton qtButtonToEngineButton(Qt::MouseButton button);
        iengine::KeyAction qtActionToEngineAction(bool pressed);
        int qtKeyToEngineKey(int qtKey);

    private:
        iengine::Engine& engine_;   // ← 引用，不拥有所有权
        iengine::WindowEventCallback eventCallback_;
        std::unique_ptr<iengine::WindowEventDispatcher> eventDispatcher_;
        bool shouldClose_;
        bool initialized_;
        
        // 连续渲染定时器
        QTimer* renderTimer_;
    };

} // namespace sandbox