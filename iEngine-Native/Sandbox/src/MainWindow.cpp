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
        qtWindow_ = std::make_shared<QtWindow>(this);
        //if (!qtWindow_->initialize()) {
        //    std::cerr << "Failed to initialize QtWindow" << std::endl;
        //    return;
        //}
        
        // 设置中心部件
        //QWidget* centralWidget = new QWidget(this);
        setCentralWidget(qtWindow_.get());
        
        // 创建布局
        //QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        //layout->setContentsMargins(0, 0, 0, 0);
        //layout->addWidget(qtWindow_->getQtWidget());
        
        // 注册Qt窗口创建器到Engine的WindowFactory
        iengine::WindowFactory::registerWindowCreator(
            iengine::WindowType::Qt,
            [this]() -> std::unique_ptr<iengine::WindowInterface> {
                auto window = std::make_unique<QtWindow>();
                //if (window->initialize()) {
                //    return window;
                //}
                return nullptr;
            }
        );
        
        std::cout << "Qt窗口已注册到Engine的WindowFactory" << std::endl;
        
        // 初始化引擎
        initializeEngine();
        
        // 创建菜单栏
        createMenus();

        // 设置场景
        setupScene();
    }
    
    MainWindow::~MainWindow() {
        // 清理资源
        // 关键：在销毁任何可能被 eventCallback_ 捕获的对象之前，
        // 先清空 QtWindow 的事件回调！
        std::cout << "MainWindow destructor called." << std::endl;
        if (qtWindow_) {
            std::cout << "qtWindow_ is valid, clearing callback..." << std::endl;
            qtWindow_->setEventCallback(nullptr);
        }
        else {
            std::cout << "qtWindow_ is nullptr!" << std::endl;
        }

        // 现在可以安全销毁 controllers、scene、engine 等
        // （shared_ptr 会自动按顺序释放）
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
            scene_ = std::make_shared<iengine::Scene>(qtWindow_);
            
            // 创建相机
            camera_ = std::make_shared<iengine::PerspectiveCamera>(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
            camera_->setPosition(0.0f, 0.0f, 5.0f);
            camera_->lookAt(0.0f, 0.0f, 0.0f);
            scene_->setActiveCamera(camera_);

            // 创建轨道控制器和第一人称控制器（使用引擎的抽象接口）
            orbitControls_ = std::make_shared<iengine::OrbitControls>(camera_, qtWindow_);
            firstPersonControls_ = std::make_shared<iengine::FirstPersonControls>(camera_, qtWindow_);
            
            //// 创建几何体
            //auto cubeGeometry = std::make_shared<iengine::Cube>(1.0f);
            //
            //// 创建图元
            //auto cubePrimitive = std::make_shared<iengine::Primitive>(iengine::PrimitiveType::TRIANGLES);
            //
            //// 创建网格
            //auto cubeMesh = std::make_shared<iengine::Mesh>(cubeGeometry, cubePrimitive);
            //
            //// 创建材质
            //iengine::BaseMaterialParams params;
            //params.name = "RedMaterial";
            //params.shaderName = "base_material";  // 使用正确的着色器名称
            //params.color = iengine::Color(1.0f, 0.0f, 0.0f, 1.0f); // 红色
            //auto material = std::make_shared<iengine::BaseMaterial>(params);
            //
            //// 创建模型
            //auto cubeModel = std::make_shared<iengine::Model>("Cube", cubeMesh, material);
            //
            //// 将模型添加到场景
            //scene_->addComponent(cubeModel);
            //
            //// 将场景添加到引擎
            //engine_->addScene("main", scene_);
            //
            //// 将场景设置到Qt窗口
            //qtWindow_->setScene(scene_);

             // 1. 创建红色三角形
            auto triangleGeometry = std::make_shared<iengine::Triangle>();
            auto trianglePrimitive = std::make_shared<iengine::Primitive>(iengine::PrimitiveType::TRIANGLES);
            auto triangleMesh = std::make_shared<iengine::Mesh>(triangleGeometry, trianglePrimitive);

            iengine::BaseMaterialParams triangleParams;
            triangleParams.name = "TriangleMaterial";
            triangleParams.shaderName = "base_material";
            triangleParams.color = iengine::Color(1.0f, 0.0f, 0.0f, 1.0f); // 红色
            auto triangleMaterial = std::make_shared<iengine::BaseMaterial>(triangleParams);

            auto triangleModel = std::make_shared<iengine::Model>("Triangle", triangleMesh, triangleMaterial);
            // 将三角形向左移动一点
            triangleModel->setPosition(-1.5f, 0.0f, 0.0f);

            // 2. 创建带贴图的立方体（参照TS版本）
            auto cubeGeometry = std::make_shared<iengine::Cube>(1.0f);
            auto cubePrimitive = std::make_shared<iengine::Primitive>(iengine::PrimitiveType::TRIANGLES);
            auto cubeMesh = std::make_shared<iengine::Mesh>(cubeGeometry, cubePrimitive);

            // 使用PbrMaterial代替BaseMaterial，参照TS版本
            iengine::PbrMaterialParams cubeParams;
            cubeParams.name = "CubeMaterial";
            cubeParams.shaderName = "base_pbr";
            cubeParams.baseColor = iengine::Color(1.0f, 1.0f, 1.0f, 1.0f); // 白色基础颜色
            cubeParams.metallic = 0.8f;
            cubeParams.roughness = 0.02f;

            // 加载贴图（使用相对路径 assets/textures/）
            iengine::TextureOptions texOptions;
            texOptions.name = "WoodTexture";
            texOptions.sourcePath = "./assets/textures/Wood048_1K-PNG/Wood048_1K-PNG_Color.png";
            auto woodTexture = std::make_shared<iengine::Texture>(texOptions);
            //// 设置纹理单元（参照Web版本）
            //woodTexture->setUnit(0);  // 外部不应该显示设置纹理单元，应该由引擎内部维护
            cubeParams.baseColorMap = woodTexture;

            std::cout << "正在创建带贴图的PBR材质..." << std::endl;
            auto cubeMaterial = std::make_shared<iengine::PbrMaterial>(cubeParams);

            auto cubeModel = std::make_shared<iengine::Model>("Cube", cubeMesh, cubeMaterial);
            // 将立方体向右移动一点
            cubeModel->setPosition(1.5f, 0.0f, 0.0f);

            // 3. 创建黄色线框立方体 (如果支持线框模式)
            auto wireframeGeometry = std::make_shared<iengine::Cube>(1.0f);
            auto wireframePrimitive = std::make_shared<iengine::Primitive>(iengine::PrimitiveType::LINES);
            auto wireframeMesh = std::make_shared<iengine::Mesh>(wireframeGeometry, wireframePrimitive);

            iengine::BaseMaterialParams wireframeParams;
            wireframeParams.name = "WireframeMaterial";
            wireframeParams.shaderName = "base_material";
            wireframeParams.color = iengine::Color(1.0f, 1.0f, 0.0f, 1.0f); // 黄色
            auto wireframeMaterial = std::make_shared<iengine::BaseMaterial>(wireframeParams);

            auto wireframeModel = std::make_shared<iengine::Model>("CubeWireframe", wireframeMesh, wireframeMaterial);
            wireframeModel->setPosition(0.0f, 2.0f, 0.0f); // 向上移动

            // 将所有模型添加到场景
            scene_->addComponent(triangleModel);
            scene_->addComponent(cubeModel);
            scene_->addComponent(wireframeModel);

            // 将场景添加到引擎
            engine_->addScene("main", scene_);

            std::cout << "场景创建成功，包含 " << scene_->getComponents().size() << " 个组件" << std::endl;

            // 注册内置着色器
            //iengine::ShaderLib::registerBuiltInShaders();
            auto shaderNames = iengine::ShaderLib::getAllShaderNames();
            std::cout << "已注册 " << shaderNames.size() << " 个着色器" << std::endl;

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