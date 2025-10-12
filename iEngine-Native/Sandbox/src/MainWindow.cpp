#include "MainWindow.h"
#include "QtWindow.h"
#include <iengine/iengine.h>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

namespace sandbox {

    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
        , qtWindow_(nullptr)
        , engine_(nullptr)
        , scene_(nullptr)
        , camera_(nullptr) {
        
        // 设置窗口属性
        setWindowTitle("iEngine Qt Sandbox");
        resize(1024, 768);
        
        // 创建Qt窗口抽象
        qtWindow_ = std::make_unique<QtWindow>();
        if (!qtWindow_->initialize()) {
            std::cerr << "Failed to initialize QtWindow" << std::endl;
            return;
        }
        
        // 设置中心部件
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // 创建布局
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(qtWindow_->getQtWidget());
        
        // 注册Qt窗口创建器到Engine的WindowFactory
        iengine::WindowFactory::registerWindowCreator(
            iengine::WindowType::Qt,
            [this]() -> std::unique_ptr<iengine::WindowInterface> {
                auto window = std::make_unique<QtWindow>();
                if (window->initialize()) {
                    return window;
                }
                return nullptr;
            }
        );
        
        std::cout << "Qt窗口已注册到Engine的WindowFactory" << std::endl;
        
        // 初始化引擎
        initializeEngine();
        
        // 设置场景
        setupScene();
        
        // 创建菜单栏
        createMenus();
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
            
            // 将引擎设置到Qt窗口
            qtWindow_->setEngine(engine_);
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize engine: " << e.what() << std::endl;
        }
    }
    
    void MainWindow::setupScene() {
        if (!engine_) {
            return;
        }
        
        try {
            // 创建场景（Qt模式，不使用GLFW窗口）
            scene_ = std::make_shared<iengine::Scene>(nullptr);
            
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
            params.shaderName = "base_material";  // 使用正确的着色器名称
            params.color = iengine::Color(1.0f, 0.0f, 0.0f, 1.0f); // 红色
            auto material = std::make_shared<iengine::BaseMaterial>(params);
            
            // 创建模型
            auto cubeModel = std::make_shared<iengine::Model>("Cube", cubeMesh, material);
            
            // 将模型添加到场景
            scene_->addComponent(cubeModel);
            
            // 将场景添加到引擎
            engine_->addScene("main", scene_);
            
            // 将场景设置到Qt窗口
            qtWindow_->setScene(scene_);
        } catch (const std::exception& e) {
            std::cerr << "Failed to setup scene: " << e.what() << std::endl;
        }
    }
    
    void MainWindow::createMenus() {
        // 创建文件菜单
        QMenu* fileMenu = menuBar()->addMenu("&File");
        
        QAction* exitAction = new QAction("&Exit", this);
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        fileMenu->addAction(exitAction);
        
        // 创建帮助菜单
        QMenu* helpMenu = menuBar()->addMenu("&Help");
        
        QAction* aboutAction = new QAction("&About", this);
        connect(aboutAction, &QAction::triggered, [this]() {
            // 简单的关于对话框
        });
        helpMenu->addAction(aboutAction);
    }

} // namespace sandbox