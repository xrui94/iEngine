#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <memory>

// 前向声明
namespace iengine {
    class Engine;
    class Scene;
}

namespace sandbox {

    class MainOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
        Q_OBJECT

    public:
        explicit MainOpenGLWidget(QWidget* parent = nullptr);
        ~MainOpenGLWidget();

        void setEngine(std::shared_ptr<iengine::Engine> engine);
        void setScene(std::shared_ptr<iengine::Scene> scene);

    protected:
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;

    private:
        std::shared_ptr<iengine::Engine> engine_;
        std::shared_ptr<iengine::Scene> scene_;
        bool initialized_;
    };

} // namespace sandbox