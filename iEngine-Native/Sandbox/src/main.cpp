#include <sandbox/MainWindow.h>

#include <QApplication>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    try {
        // 创建 Qt 应用
        QApplication app(argc, argv);
        
        // 设置应用信息
        app.setApplicationName("iEngine Sandbox");
        app.setApplicationVersion("1.0.0");
        
        // 创建主窗口
        sandbox::MainWindow mainWindow;
        mainWindow.show();
        
        // 运行应用
        return app.exec();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    }
}