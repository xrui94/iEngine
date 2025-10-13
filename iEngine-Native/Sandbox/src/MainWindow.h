#pragma once

#include <QMainWindow>
#include <memory>

// 前向声明
namespace iengine {
    class Engine;
    class Scene;
    class PerspectiveCamera;
	class OrbitControls;
	class FirstPersonControls;
    class WindowInterface;  // 添加 WindowInterface 的前向声明
}

namespace sandbox {
    class QtWindow;

    class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow();

    private:
        void initializeEngine();
        void setupScene();
        void createMenus();

        // std::shared_ptr<QtWindow> qtWindow_;
        // 上边的qtWindow_成员变量会导致窗口关闭时崩溃，原因如下：
        // QtWindow 继承自 QOpenGLWidget，它的生命周期由 Qt 的父子关系管理。
        // 当 MainWindow 析构时，Qt 会自动删除子 widget（QtWindow），但
        //  MainWindow 同时也持有 qtWindow_ 的 shared_ptr，这导致了双重
        // 删除或引用计数管理混乱。
        // 解决方案
        // 1.使用原始指针管理 QtWindow：
        // - MainWindow 中的 qtWindow_ 改为原始指针
        // - 让 Qt 的父子关系完全管理 QtWindow 的生命周期
        // 2.创建非拥有的 shared_ptr 包装器：
        // - 添加 qtWindowWrapper_ 成员
        // - 使用自定义的空删除器：[](iengine::WindowInterface*) { /* 不删除，Qt会管理 */ }
        // - 这个包装器只用于传递给引擎和场景，不拥有实际的所有权
        // 3.严格控制析构顺序：
        // - 在 MainWindow 析构时，先清理事件监听器
        // - 清理 QtWindow 持有的 Engine 和 Scene 引用
        // - 清理控制器、相机、场景、引擎
        // - 最后让 Qt 自动清理 QtWindow
        QtWindow* qtWindow_;  // Qt 通过父子关系管理生命周期
        std::shared_ptr<iengine::WindowInterface> qtWindowWrapper_;  // 用于传递给引擎，但不拥有所有权
        std::shared_ptr<iengine::Engine> engine_;
        std::shared_ptr<iengine::Scene> scene_;
        std::shared_ptr<iengine::PerspectiveCamera> camera_;
        std::shared_ptr<iengine::OrbitControls> orbitControls_;        // 轨道控制器
        std::shared_ptr<iengine::FirstPersonControls> firstPersonControls_; // 第一人称控制器
    };

} // namespace sandbox