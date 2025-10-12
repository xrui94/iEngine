#include "MainWindow.h"

#include <QApplication>

#include <iostream>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

// 设置控制台编码以支持中文输出
void setupConsoleEncoding() {
#ifdef _WIN32
    // 设置控制台输出编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 设置cout和cerr使用UTF-8
    std::locale::global(std::locale(".UTF8"));
#endif
}

int main(int argc, char *argv[]) {
    // 首先设置控制台编码
    setupConsoleEncoding();
    
    try {
        std::cout << "=== iEngine Qt 应用程序示例 ===" << std::endl;
        
        // 创建 Qt 应用
        QApplication app(argc, argv);
        
        // 设置应用信息
        app.setApplicationName("iEngine Sandbox");
        app.setApplicationVersion("1.0.0");
        
        std::cout << "创建 iEngine Qt 主窗口..." << std::endl;
        
        // 创建主窗口
        sandbox::MainWindow mainWindow;
        mainWindow.show();
        
        std::cout << "Qt 应用程序启动成功，窗口已显示" << std::endl;
        
        // 运行应用
        int result = app.exec();
        
        std::cout << "Qt 应用程序已退出" << std::endl;
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Qt 应用程序异常: " << e.what() << std::endl;
        return -1;
    }
}