#include "iengine/windowing/Window.h"
#include <iostream>
#include <algorithm>

namespace iengine {

    void WindowEventDispatcher::addEventListener(std::weak_ptr<WindowEventListener> listener) {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        
        // 检查是否已经存在
        auto sharedListener = listener.lock();
        if (!sharedListener) {
            return; // 无效的weak_ptr
        }

        for (const auto& existingListener : listeners_) {
            auto existing = existingListener.lock();
            if (existing && existing.get() == sharedListener.get()) {
                return; // 已经存在，不重复添加
            }
        }

        listeners_.push_back(listener);
        sortListenersByPriority();
        
        std::cout << "WindowEventDispatcher: 添加事件监听器，当前监听器数量: " << listeners_.size() << std::endl;
    }

    void WindowEventDispatcher::removeEventListener(std::weak_ptr<WindowEventListener> listener) {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        
        auto sharedListener = listener.lock();
        if (!sharedListener) {
            return; // 无效的weak_ptr
        }

        listeners_.erase(
            std::remove_if(listeners_.begin(), listeners_.end(),
                [&sharedListener](const std::weak_ptr<WindowEventListener>& weakListener) {
                    auto existing = weakListener.lock();
                    return !existing || existing.get() == sharedListener.get();
                }),
            listeners_.end()
        );
        
        std::cout << "WindowEventDispatcher: 移除事件监听器，当前监听器数量: " << listeners_.size() << std::endl;
    }

    void WindowEventDispatcher::clearEventListeners() {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        listeners_.clear();
        std::cout << "WindowEventDispatcher: 清除所有事件监听器" << std::endl;
    }

    bool WindowEventDispatcher::dispatchEvent(const WindowEvent& event) {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        
        // 清理过期的监听器
        cleanupExpiredListeners();
        
        bool handled = false;
        
        // 按优先级分发事件
        for (const auto& weakListener : listeners_) {
            auto listener = weakListener.lock();
            if (listener) {
                try {
                    if (listener->onWindowEvent(event)) {
                        handled = true;
                        break; // 事件被处理，停止传播
                    }
                } catch (const std::exception& e) {
                    std::cerr << "WindowEventDispatcher: 监听器处理事件时发生异常: " << e.what() << std::endl;
                }
            }
        }
        
        return handled;
    }

    size_t WindowEventDispatcher::getListenerCount() const {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        return listeners_.size();
    }

    void WindowEventDispatcher::cleanupExpiredListeners() {
        // 注意：此函数假设已经获得了锁
        listeners_.erase(
            std::remove_if(listeners_.begin(), listeners_.end(),
                [](const std::weak_ptr<WindowEventListener>& weakListener) {
                    return weakListener.expired();
                }),
            listeners_.end()
        );
    }

    void WindowEventDispatcher::sortListenersByPriority() {
        // 注意：此函数假设已经获得了锁
        std::sort(listeners_.begin(), listeners_.end(),
            [](const std::weak_ptr<WindowEventListener>& a, const std::weak_ptr<WindowEventListener>& b) {
                auto sharedA = a.lock();
                auto sharedB = b.lock();
                
                if (!sharedA) return false;
                if (!sharedB) return true;
                
                return sharedA->getPriority() < sharedB->getPriority();
            });
    }

} // namespace iengine