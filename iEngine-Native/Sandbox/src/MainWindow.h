#pragma once

#include <QMainWindow>
#include <memory>

// 前向声明
namespace iengine {
    class Engine;
    class Scene;
    class PerspectiveCamera;
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

        std::unique_ptr<QtWindow> qtWindow_;
        std::shared_ptr<iengine::Engine> engine_;
        std::shared_ptr<iengine::Scene> scene_;
        std::shared_ptr<iengine::PerspectiveCamera> camera_;
    };

} // namespace sandbox