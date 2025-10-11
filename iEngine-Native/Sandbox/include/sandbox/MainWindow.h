#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <memory>

// 前向声明
namespace iengine {
    class Engine;
    class Scene;
    class PerspectiveCamera;
}

namespace sandbox {

    class MainOpenGLWidget;

    class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow();

        void initializeEngine();
        void setupScene();

    private:
        MainOpenGLWidget* openGLWidget_;
        std::shared_ptr<iengine::Engine> engine_;
        std::shared_ptr<iengine::Scene> scene_;
        std::shared_ptr<iengine::PerspectiveCamera> camera_;
    };

} // namespace sandbox