#pragma once

#include <memory>
#include <string>
#include <functional>

namespace iengine {
    // 前向声明
    class Context;
    
    // 窗口事件类型（扩展支持鼠标键盘交互）
    enum class WindowEventType {
        Resize,
        Close,
        MouseButton,
        MouseMove,
        MouseScroll,
        Key
    };
    
    // 鼠标按键枚举
    enum class MouseButton {
        Left = 0,
        Right = 1,
        Middle = 2
    };
    
    // 按键动作枚举
    enum class KeyAction {
        Release = 0,
        Press = 1,
        Repeat = 2
    };
    
    // 窗口事件结构（扩展版）
    struct WindowEvent {
        WindowEventType type;
        union {
            struct { int width, height; } resize;
            struct { MouseButton button; KeyAction action; double x, y; } mouseButton;
            struct { double x, y; } mouseMove;
            struct { double xoffset, yoffset; } mouseScroll;
            struct { int key; KeyAction action; int mods; } key;
        } data;
    };
    
    // 窗口配置（基本配置）
    struct WindowConfig {
        std::string title = "iEngine Window";
        int width = 1024;
        int height = 768;
        bool fullscreen = false;
    };
    
    // 窗口事件回调函数类型
    using WindowEventCallback = std::function<void(const WindowEvent&)>;
    
    // 最小化的窗口抽象接口（仅提供引擎需要的基本功能）
    class WindowInterface {
    public:
        virtual ~WindowInterface() = default;
        
        // 引擎需要的基本窗口信息
        virtual void getSize(int& width, int& height) const = 0;
        virtual bool shouldClose() const = 0;
        
        // 上下文管理（引擎核心需要）
        virtual std::shared_ptr<Context> getContext() const = 0;
        virtual void makeContextCurrent() = 0;
        
        // 事件回调（供引擎注册）
        virtual void setEventCallback(const WindowEventCallback& callback) = 0;
    };
    
} // namespace iengine