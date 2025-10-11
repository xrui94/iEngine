#include <sandbox/MainWindow.h>
#include <sandbox/MainOpenGLWidget.h>
#include <iengine/iengine.h>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <iostream>

namespace sandbox {

    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
        , openGLWidget_(new MainOpenGLWidget(this))
        , engine_(nullptr)
        , scene_(nullptr)
        , camera_(nullptr) {
        
        // 设置窗口属性
        setWindowTitle("iEngine Sandbox");
        resize(1024, 768);
        
        // 创建中心部件
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // 创建布局
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(openGLWidget_);
        
        // 初始化引擎
        initializeEngine();
        
        // 设置场景
        setupScene();
        
        // 创建菜单栏
        QMenuBar* menuBar = new QMenuBar(this);
        setMenuBar(menuBar);
        
        // 文件菜单
        QMenu* fileMenu = menuBar->addMenu("&File");
        QAction* exitAction = fileMenu->addAction("&Exit");
        connect(exitAction, &QAction::triggered, this, &QApplication::quit);
        
        // 视图菜单
        QMenu* viewMenu = menuBar->addMenu("&View");
        QAction* resetViewAction = viewMenu->addAction("&Reset View");
        connect(resetViewAction, &QAction::triggered, this, [this]() {
            if (camera_) {
                camera_->setPosition(0.0f, 0.0f, 5.0f);
                camera_->lookAt(0.0f, 0.0f, 0.0f);
            }
        });
    }
    
    MainWindow::~MainWindow() {
        // 清理资源
    }
    
    void MainWindow::initializeEngine() {
        try {
            // 创建引擎实例
            iengine::EngineOptions options;
            options.renderer = iengine::RendererType::OpenGL;
            engine_ = std::make_shared<iengine::Engine>(options);
            
            // 将引擎设置到 OpenGL widget
            openGLWidget_->setEngine(engine_);
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize engine: " << e.what() << std::endl;
        }
    }
    
    void MainWindow::setupScene() {
        if (!engine_) {
            return;
        }
        
        try {
            // 创建场景
            scene_ = std::make_shared<iengine::Scene>();
            
            // 创建相机
            camera_ = std::make_shared<iengine::PerspectiveCamera>(60.0f, 1.0f, 0.1f, 100.0f);
            camera_->setPosition(0.0f, 0.0f, 5.0f);
            camera_->lookAt(0.0f, 0.0f, 0.0f);
            scene_->setActiveCamera(camera_);
            
            // 创建几何体
            auto cubeGeometry = std::make_shared<iengine::Cube>(1.0f);
            
            // 创建图元
            auto cubePrimitive = std::make_shared<iengine::Primitive>(iengine::PrimitiveType::TRIANGLES);
            
            // 创建网格
            auto cubeMesh = std::make_shared<iengine::Mesh>(cubeGeometry, cubePrimitive);
            
            // 创建材质
            iengine::BaseMaterialParams params;
            params.name = "RedMaterial";
            params.shaderName = "basic";
            params.color = iengine::Color(1.0f, 0.0f, 0.0f, 1.0f); // 红色
            auto material = std::make_shared<iengine::BaseMaterial>(params);
            
            // 创建模型
            auto cubeModel = std::make_shared<iengine::Model>("Cube", cubeMesh, material);
            
            // 将模型添加到场景
            scene_->addComponent(cubeModel);
            
            // 将场景添加到引擎
            engine_->addScene("main", scene_);
            engine_->setActiveScene("main");
            
            // 将场景设置到 OpenGL widget
            openGLWidget_->setScene(scene_);
        } catch (const std::exception& e) {
            std::cerr << "Failed to setup scene: " << e.what() << std::endl;
        }
    }

} // namespace sandbox