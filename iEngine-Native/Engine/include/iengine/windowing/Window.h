#pragma once

#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <mutex>

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
    
    // 窗口事件回调函数类型（保留向后兼容）
    using WindowEventCallback = std::function<void(const WindowEvent&)>;
    
    /**
     * @brief 窗口事件监听器接口
     * 
     * 采用观察者模式，避免std::function回调导致的生命周期问题
     */
    class WindowEventListener {
    public:
        virtual ~WindowEventListener() = default;
        
        /**
         * @brief 处理窗口事件
         * @param event 窗口事件
         * @return true表示事件已处理，停止传播；false表示继续传播
         */
        virtual bool onWindowEvent(const WindowEvent& event) = 0;
        
        /**
         * @brief 获取监听器优先级（数值越小优先级越高）
         * @return 优先级值
         */
        virtual int getPriority() const { return 0; }
    };
    
    /**
     * @brief 窗口事件分发器
     * 
     * 负责管理事件监听器的注册、注销和事件分发
     * 线程安全，支持监听器优先级
     */
    class WindowEventDispatcher {
    public:
        WindowEventDispatcher() = default;
        ~WindowEventDispatcher() = default;

        // 禁止拷贝和移动
        WindowEventDispatcher(const WindowEventDispatcher&) = delete;
        WindowEventDispatcher& operator=(const WindowEventDispatcher&) = delete;
        WindowEventDispatcher(WindowEventDispatcher&&) = delete;
        WindowEventDispatcher& operator=(WindowEventDispatcher&&) = delete;

        /**
         * @brief 添加事件监听器
         * @param listener 监听器（弱引用，需要外部保证生命周期）
         */
        void addEventListener(std::weak_ptr<WindowEventListener> listener);

        /**
         * @brief 移除事件监听器
         * @param listener 要移除的监听器
         */
        void removeEventListener(std::weak_ptr<WindowEventListener> listener);

        /**
         * @brief 移除所有事件监听器
         */
        void clearEventListeners();

        /**
         * @brief 分发事件给所有监听器
         * @param event 要分发的事件
         * @return true表示有监听器处理了事件，false表示无监听器处理
         */
        bool dispatchEvent(const WindowEvent& event);

        /**
         * @brief 获取当前监听器数量
         * @return 监听器数量
         */
        size_t getListenerCount() const;

    private:
        mutable std::mutex listenersMutex_;
        std::vector<std::weak_ptr<WindowEventListener>> listeners_;

        /**
         * @brief 清理已失效的监听器
         */
        void cleanupExpiredListeners();

        /**
         * @brief 按优先级排序监听器
         */
        void sortListenersByPriority();
    };
    
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
        
        // 事件回调（供引擎注册，保留向后兼容）
        virtual void setEventCallback(const WindowEventCallback& callback) = 0;
        
        /**
         * @brief 获取事件分发器（新的观察者模式接口）
         * @return 事件分发器引用
         */
        virtual WindowEventDispatcher& getEventDispatcher() = 0;
        virtual const WindowEventDispatcher& getEventDispatcher() const = 0;

        /**
         * @brief 添加事件监听器（便捷方法）
         * @param listener 监听器
         */
        void addEventListener(std::weak_ptr<WindowEventListener> listener) {
            getEventDispatcher().addEventListener(listener);
        }

        /**
         * @brief 移除事件监听器（便捷方法）
         * @param listener 要移除的监听器
         */
        void removeEventListener(std::weak_ptr<WindowEventListener> listener) {
            getEventDispatcher().removeEventListener(listener);
        }
    };
    
} // namespace iengine