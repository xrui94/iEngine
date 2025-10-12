#pragma once

#include "Window.h"
#include <memory>
#include <functional>
#include <vector>
#include <map>

namespace iengine {
    
    // 窗口类型枚举（简化）
    enum class WindowType {
        Custom = 0,  // 自定义窗口实现
        GLFW = 1,
        Qt = 2
    };
    
    // 窗口创建函数类型（返回WindowInterface指针）
    using WindowCreatorFunction = std::function<std::unique_ptr<WindowInterface>()>;
    
    // 简化的窗口工厂（仅供注册和创建）
    class WindowFactory {
    public:
        // 注册窗口创建器
        static void registerWindowCreator(WindowType type, WindowCreatorFunction creator);
        
        // 创建指定类型的窗口
        static std::unique_ptr<WindowInterface> createWindow(WindowType type);
        
        // 检查窗口类型是否可用
        static bool isWindowTypeAvailable(WindowType type);
        
        // 获取所有可用的窗口类型
        static std::vector<WindowType> getAvailableWindowTypes();
        
        // 清理所有注册的创建器（主要用于清理或测试）
        static void clearCreators();
        
    private:
        WindowFactory() = delete;
        
        static std::map<WindowType, WindowCreatorFunction> creators_;
    };
    
} // namespace iengine