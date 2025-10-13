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

        std::shared_ptr<QtWindow> qtWindow_;
        std::shared_ptr<iengine::Engine> engine_;
        std::shared_ptr<iengine::Scene> scene_;
        std::shared_ptr<iengine::PerspectiveCamera> camera_;
        std::shared_ptr<iengine::OrbitControls> orbitControls_;        // 轨道控制器
        std::shared_ptr<iengine::FirstPersonControls> firstPersonControls_; // 第一人称控制器
    };

} // namespace sandbox