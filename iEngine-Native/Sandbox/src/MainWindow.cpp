#include "MainWindow.h"
#include "QtWindow.h"
#include <iengine/iengine.h>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QToolBar>
#include <QButtonGroup>
#include <QActionGroup>
#include <iostream>

namespace sandbox {

    // 全局事件监听器（P键切换控制器）
    class GlobalEventListener : public iengine::WindowEventListener {
    public:
        explicit GlobalEventListener(MainWindow* mainWindow) : mainWindow_(mainWindow) {}
        
        bool onWindowEvent(const iengine::WindowEvent& event) override {
            // 监听P键按下事件
            if (event.type == iengine::WindowEventType::Key && 
                event.data.key.action == iengine::KeyAction::Press &&
                event.data.key.key == 80) { // P键的键码是80
                
                if (mainWindow_) {
                    mainWindow_->switchControllerMode();
                }
                return true; // 阻止事件继续传播
            }
            return false; // 允许其他监听器处理
        }
        
        int getPriority() const override {
            return 0; // 最高优先级，在控制器之前处理
        }
        
    private:
        MainWindow* mainWindow_;
    };

    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
        , qtWindow_(nullptr)
        , qtWindowWrapper_(nullptr)
        , engine_(nullptr)
        , scene_(nullptr)
        , camera_(nullptr)
        , isFirstPersonMode_(false) {
        
        // 设置窗口属性
        setWindowTitle("iEngine Qt Sandbox");
        resize(1024, 768);
        
        // 创建Qt窗口抽象（Qt通过父子关系管理生命周期）
        qtWindow_ = new QtWindow(this);
        
        // 创建一个不拥有所有权的 shared_ptr 包装器（使用空删除器）
        qtWindowWrapper_ = std::shared_ptr<iengine::WindowInterface>(
            qtWindow_, [](iengine::WindowInterface*) { /* 不删除，Qt会管理 */ }
        );
        
        // 设置中心部件
        setCentralWidget(qtWindow_);
        
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
        
        // 创建工具栏
        createToolBar();

        // 设置场景
        setupScene();
    }
    
    MainWindow::~MainWindow() {
        std::cout << "MainWindow: 开始析构" << std::endl;
        
        // 关键：在 Qt 删除 widget 之前先清理所有依赖
        // 1. 首先清理事件监听器（避免悬空指针）
        if (qtWindow_) {
            std::cout << "MainWindow: 清理QtWindow事件回调和监听器" << std::endl;
            qtWindow_->setEventCallback(nullptr);
            qtWindow_->getEventDispatcher().clearEventListeners();
            
            // 2. 清理 QtWindow 持有的 Engine 和 Scene 引用
            std::cout << "MainWindow: 清理QtWindow持有的引擎和场景引用" << std::endl;
            qtWindow_->setEngine(nullptr);
            qtWindow_->setScene(nullptr);
        }
        
        // 3. 清理控制器（它们持有window和camera的引用）
        std::cout << "MainWindow: 清理控制器" << std::endl;
        orbitControls_.reset();
        firstPersonControls_.reset();
        
        // 4. 清理相机
        std::cout << "MainWindow: 清理相机" << std::endl;
        camera_.reset();
        
        // 5. 清理场景（它持有Context引用）
        std::cout << "MainWindow: 清理场景" << std::endl;
        scene_.reset();
        
        // 6. 清理引擎
        std::cout << "MainWindow: 清理引擎" << std::endl;
        engine_.reset();
        
        // 7. Qt 会自动删除 qtWindow_（通过父子关系）
        // 我们不需要手动删除
        std::cout << "MainWindow: 等待Qt清理QtWindow" << std::endl;
        
        std::cout << "MainWindow: 析构完成" << std::endl;
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
            // 创建场景（使用包装器）
            scene_ = std::make_shared<iengine::Scene>(qtWindowWrapper_);
            
            // 创建相机
            camera_ = std::make_shared<iengine::PerspectiveCamera>(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
            camera_->setPosition(0.0f, 0.0f, 5.0f);
            camera_->lookAt(0.0f, 0.0f, 0.0f);
            scene_->setActiveCamera(camera_);

            // 创建轨道控制器和第一人称控制器（使用包装器）
            orbitControls_ = std::make_shared<iengine::OrbitControls>(camera_, qtWindowWrapper_);
            firstPersonControls_ = std::make_shared<iengine::FirstPersonControls>(camera_, qtWindowWrapper_);
            
            // 初始化为轨道控制器模式
            qtWindow_->addEventListener(orbitControls_);
            
            // 创建并注册全局事件监听器（P键切换）
            globalEventListener_ = std::make_shared<GlobalEventListener>(this);
            qtWindow_->addEventListener(globalEventListener_);
            
            std::cout << "当前模式：轨道控制器 - 鼠标拖拽旋转、滚轮缩放\n快捷键：P键切换" << std::endl;
            
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
    
    void MainWindow::createToolBar() {
        // 创建主工具栏
        QToolBar* mainToolBar = addToolBar("控制器");
        mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        
        // 轨道控制器按钮
        QAction* orbitAction = new QAction("轨道控制", this);
        orbitAction->setCheckable(true);
        orbitAction->setChecked(true); // 默认选中
        orbitAction->setToolTip("轨道控制器 - 鼠标拖拽旋转、滚轮缩放\n快捷键：P键切换");
        connect(orbitAction, &QAction::triggered, this, &MainWindow::onOrbitControlsSelected);
        
        // 第一人称控制器按钮
        QAction* fpAction = new QAction("第一人称", this);
        fpAction->setCheckable(true);
        fpAction->setToolTip("第一人称控制器 - WASD移动、鼠标看向\n快捷键：P键切换");
        connect(fpAction, &QAction::triggered, this, &MainWindow::onFirstPersonControlsSelected);
        
        // 添加到工具栏
        mainToolBar->addAction(orbitAction);
        mainToolBar->addAction(fpAction);
        
        // 创建按钮组实现互斥
        QActionGroup* controllerGroup = new QActionGroup(this);
        controllerGroup->addAction(orbitAction);
        controllerGroup->addAction(fpAction);
        controllerGroup->setExclusive(true);
        
        // 添加分隔符
        mainToolBar->addSeparator();
        
        // 关于按钮
        QAction* aboutAction = new QAction("关于", this);
        aboutAction->setToolTip("关于 iEngine");
        connect(aboutAction, &QAction::triggered, [this]() {
            // TODO: 显示关于对话框
        });
        mainToolBar->addAction(aboutAction);
    }
    
    void MainWindow::switchControllerMode() {
        if (isFirstPersonMode_) {
            // 切换到轨道控制器
            qtWindow_->removeEventListener(firstPersonControls_);
            qtWindow_->addEventListener(orbitControls_);
            isFirstPersonMode_ = false;
            std::cout << "切换到轨道控制器模式 - 鼠标拖拽旋转、滚轮缩放" << std::endl;
        } else {
            // 切换到第一人称控制器
            qtWindow_->removeEventListener(orbitControls_);
            qtWindow_->addEventListener(firstPersonControls_);
            isFirstPersonMode_ = true;
            std::cout << "切换到第一人称控制器模式 - WASD移动、鼠标看向" << std::endl;
        }
    }
    
    void MainWindow::onOrbitControlsSelected() {
        if (isFirstPersonMode_) {
            switchControllerMode();
        }
    }
    
    void MainWindow::onFirstPersonControlsSelected() {
        if (!isFirstPersonMode_) {
            switchControllerMode();
        }
    }

} // namespace sandbox