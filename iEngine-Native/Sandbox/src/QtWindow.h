#pragma once

#include <glad/glad.h>  // GLAD必须在所有OpenGL相关头文件之前
#include <iengine/iengine.h>
#include <QOpenGLWidget>
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
    class QtWindow : public QOpenGLWidget, public iengine::WindowInterface {
        Q_OBJECT
    public:
        QtWindow(QWidget* parent = nullptr);
        ~QtWindow() override;

        /**
         * @brief 初始化Qt OpenGL窗口
         * @param parent 父Widget
         * @return 初始化是否成功
         */
        //bool initialize(QWidget* parent = nullptr);

        // WindowInterface 接口实现
        void getSize(int& width, int& height) const override;
        bool shouldClose() const override;
        std::shared_ptr<iengine::Context> getContext() const override;
        void makeContextCurrent() override;
        void setEventCallback(const iengine::WindowEventCallback& callback) override;

        // Qt特有的方法
        //QOpenGLWidget* getQtWidget() const { return widget_; }
        void setEngine(std::shared_ptr<iengine::Engine> engine);
        void setScene(std::shared_ptr<iengine::Scene> scene);

        // 渲染相关
        void render();

	protected:
        // 内部初始化方法 - 设为公有以便内部Widget调用
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;

    private:
        //QOpenGLWidget* widget_;
        std::shared_ptr<iengine::Context> context_;
        iengine::WindowEventCallback eventCallback_;
        std::shared_ptr<iengine::Engine> engine_;
        std::shared_ptr<iengine::Scene> scene_;
        bool shouldClose_;
        bool initialized_;
    };

} // namespace sandbox