#include <iengine/iengine.h>
#include <iostream>

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

// 简化的引擎测试函数
void demonstrateEngineCore() {
    std::cout << "=== iEngine 核心系统测试 ===" << std::endl;
    
    try {
        // 测试引擎创建
        std::cout << "创建引擎实例..." << std::endl;
        iengine::EngineOptions options;
        options.renderer = iengine::RendererType::OpenGL;
        auto engine = std::make_shared<iengine::Engine>(options);
        std::cout << "引擎创建成功" << std::endl;
        
        // 测试着色器库
        std::cout << "初始化着色器库..." << std::endl;
        iengine::ShaderLib::registerBuiltInShaders();
        auto shaderNames = iengine::ShaderLib::getAllShaderNames();
        std::cout << "已注册 " << shaderNames.size() << " 个着色器:" << std::endl;
        for (const auto& name : shaderNames) {
            std::cout << "  - " << name << std::endl;
        }
        
        std::cout << "引擎核心系统测试完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "测试过程中发生异常: " << e.what() << std::endl;
        throw;
    }
}

// 独立的引擎核心测试程序的main函数
int main() {
    // 首先设置控制台编码
    setupConsoleEncoding();
    
    try {
        std::cout << "启动引擎核心测试程序..." << std::endl;
        
        // 运行引擎核心测试
        demonstrateEngineCore();
        
        std::cout << "所有测试通过！" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return -1;
    }
}