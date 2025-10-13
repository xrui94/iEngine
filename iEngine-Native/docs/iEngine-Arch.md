# iEngine 事件系统架构设计文档

> **版本**: 1.0  
> **更新日期**: 2025-10-13  
> **作者**: iEngine开发团队

---

## 目录

1. [概述](#1-概述)
2. [问题背景与解决方案](#2-问题背景与解决方案)
3. [观察者模式设计](#3-观察者模式设计)
4. [Qt窗口生命周期管理](#4-qt窗口生命周期管理)
5. [API使用指南](#5-api使用指南)
6. [迁移指南](#6-迁移指南)
7. [性能与测试](#7-性能与测试)

---

## 1. 概述

iEngine采用**观察者模式（Observer Pattern）**重构了事件系统，彻底解决了原有`std::function`回调带来的生命周期管理问题，特别是Qt窗口关闭时的崩溃bug。

### 核心特性

✅ **生命周期安全** - 使用`weak_ptr`避免悬空指针  
✅ **多监听器支持** - 支持多个组件同时监听事件  
✅ **优先级控制** - 精细控制事件处理顺序  
✅ **事件传播控制** - 类似DOM事件的传播机制  
✅ **线程安全** - 完整的并发保护  
✅ **自动内存管理** - 自动清理过期监听器  

---

## 2. 问题背景与解决方案

### 2.1 原有问题

#### 🔴 崩溃问题
```cpp
// ❌ 危险：this可能已经析构
window->setEventCallback([this](const WindowEvent& event) {
    this->handleEvent(event);  // 崩溃！
});
```

**崩溃堆栈**:
```
QtAppExample.exe!std::_Ref_count_base::_Decref() 行 1159
QtAppExample.exe!std::shared_ptr<iengine::Context>::~shared_ptr() 行 1690
QtAppExample.exe!sandbox::QtWindow::~QtWindow() 行 88
```

#### 🔴 其他问题
- 只支持单一回调函数
- 无法控制事件处理顺序
- 存在循环引用风险
- 不支持多线程

### 2.2 解决方案架构

```
┌─────────────────────────────────────┐
│         Window (主题)               │
│  ┌──────────────────────────────┐   │
│  │  WindowEventDispatcher       │   │
│  │  - listeners: weak_ptr<>     │   │
│  │  - dispatchEvent()           │   │
│  └──────────────────────────────┘   │
└────────────┬────────────────────────┘
             │ weak_ptr
             ▼
    ┌────────────────────┐
    │ WindowEventListener │
    │ - onWindowEvent()   │
    │ - getPriority()     │
    └────────────────────┘
             △
             │
    ┌────────┴────────┐
    │                 │
OrbitControls  FirstPersonControls
```

---

## 3. 观察者模式设计

### 3.1 核心组件

#### WindowEventListener（监听器接口）

```cpp
class WindowEventListener {
public:
    virtual ~WindowEventListener() = default;
    
    // 处理事件，返回true阻止传播
    virtual bool onWindowEvent(const WindowEvent& event) = 0;
    
    // 优先级：数值越小越优先（0最高）
    virtual int getPriority() const { return 0; }
};
```

#### WindowEventDispatcher（分发器）

```cpp
class WindowEventDispatcher {
public:
    void addEventListener(std::weak_ptr<WindowEventListener> listener);
    void removeEventListener(std::weak_ptr<WindowEventListener> listener);
    void clearEventListeners();
    bool dispatchEvent(const WindowEvent& event);
    size_t getListenerCount() const;

private:
    mutable std::mutex listenersMutex_;
    std::vector<std::weak_ptr<WindowEventListener>> listeners_;
    
    void cleanupExpiredListeners();
    void sortListenersByPriority();
};
```

**关键特性**:
- ✅ 使用`weak_ptr`避免循环引用
- ✅ 自动清理已析构的监听器
- ✅ 线程安全（`std::mutex`保护）
- ✅ 按优先级排序执行

### 3.2 事件分发流程

```
1. 平台事件 (Qt/GLFW)
   ↓
2. 转换为WindowEvent
   ↓
3. dispatchEvent()
   ↓
4. 清理过期监听器
   ↓
5. 按优先级排序
   ↓
6. 遍历执行监听器
   ↓
7. 检查是否阻止传播
```

### 3.3 控制器实现示例

```cpp
class OrbitControls : public BaseControls, public WindowEventListener {
public:
    // 新接口
    bool onWindowEvent(const WindowEvent& event) override {
        handleWindowEvent(event);  // 调用现有逻辑
        return false;  // 不阻止传播
    }
    
    int getPriority() const override { return 100; }
    
    // 保留旧接口（向后兼容）
    void handleWindowEvent(const WindowEvent& event);
};
```

---

## 4. Qt窗口生命周期管理

### 4.1 问题根源

`QtWindow`继承自`QOpenGLWidget`，存在**双重所有权管理**冲突：

```cpp
// ❌ 错误：双重删除导致崩溃
class MainWindow {
    std::shared_ptr<QtWindow> qtWindow_;  // shared_ptr管理
    
    MainWindow() {
        qtWindow_ = std::make_shared<QtWindow>(this);  // Qt也管理
        setCentralWidget(qtWindow_.get());
    }
    // 析构时：shared_ptr删除 + Qt删除 = 双重删除 → 崩溃
};
```

### 4.2 解决方案：混合管理模式

```cpp
class MainWindow : public QMainWindow {
private:
    QtWindow* qtWindow_;  // ✅ 原始指针，Qt管理
    std::shared_ptr<iengine::WindowInterface> qtWindowWrapper_;  // ✅ 非拥有包装器
    
public:
    MainWindow(QWidget* parent = nullptr) {
        // 1. 创建Qt窗口（Qt管理生命周期）
        qtWindow_ = new QtWindow(this);
        
        // 2. 创建非拥有包装器（空删除器）
        qtWindowWrapper_ = std::shared_ptr<iengine::WindowInterface>(
            qtWindow_, 
            [](iengine::WindowInterface*) { /* 不删除 */ }
        );
        
        // 3. 设置为中心部件
        setCentralWidget(qtWindow_);
    }
    
    ~MainWindow() {
        // 严格析构顺序
        if (qtWindow_) {
            qtWindow_->setEventCallback(nullptr);
            qtWindow_->getEventDispatcher().clearEventListeners();
            qtWindow_->setEngine(nullptr);
            qtWindow_->setScene(nullptr);
        }
        
        orbitControls_.reset();
        firstPersonControls_.reset();
        camera_.reset();
        scene_.reset();
        engine_.reset();
        
        // Qt自动清理qtWindow_
    }
};
```

**关键要点**:
1. 原始指针由Qt父子关系管理
2. 非拥有包装器用于传递给引擎（空删除器）
3. 严格按依赖关系逆序析构
4. 析构前清理所有交叉引用

### 4.3 QtWindow安全析构

```cpp
QtWindow::~QtWindow() {
    if (eventDispatcher_) {
        eventDispatcher_->clearEventListeners();
    }
    
    scene_.reset();
    engine_.reset();
    
    if (context_) {
        std::cout << "Context use_count=" << context_.use_count() << std::endl;
        context_.reset();
    }
}
```

---

## 5. API使用指南

### 5.1 基本用法

```cpp
// 1. 创建控制器
auto orbitControls = std::make_shared<OrbitControls>(camera, window);

// 2. 注册监听器
window->addEventListener(orbitControls);

// 3. 移除监听器
window->removeEventListener(orbitControls);

// 4. 清理所有监听器
window->getEventDispatcher().clearEventListeners();
```

### 5.2 自定义监听器

```cpp
class CustomListener : public WindowEventListener {
public:
    bool onWindowEvent(const WindowEvent& event) override {
        if (event.type == WindowEventType::Key && 
            event.data.key.key == 80) {
            handlePKey();
            return true;  // 阻止传播
        }
        return false;
    }
    
    int getPriority() const override { return 0; }  // 最高优先级
};

auto listener = std::make_shared<CustomListener>();
window->addEventListener(listener);
```

### 5.3 优先级控制

```cpp
class UIHandler : public WindowEventListener {
    int getPriority() const override { return 10; }  // 高优先级
};

class GameplayHandler : public WindowEventListener {
    int getPriority() const override { return 100; }  // 中优先级
};

class LogHandler : public WindowEventListener {
    int getPriority() const override { return 200; }  // 低优先级
};

// 执行顺序：UI → Gameplay → Log
```

### 5.4 控制器切换

```cpp
void switchControllerMode() {
    if (isFirstPersonMode) {
        window->removeEventListener(firstPersonControls);
        window->addEventListener(orbitControls);
    } else {
        window->removeEventListener(orbitControls);
        window->addEventListener(firstPersonControls);
    }
    isFirstPersonMode = !isFirstPersonMode;
}
```

---

## 6. 迁移指南

### 6.1 从旧系统迁移

```cpp
// ❌ 旧系统
window->setEventCallback([this](const WindowEvent& event) {
    this->handleEvent(event);
});

// ✅ 新系统
class MyController : public WindowEventListener {
    bool onWindowEvent(const WindowEvent& event) override {
        handleEvent(event);
        return false;
    }
};
auto controller = std::make_shared<MyController>();
window->addEventListener(controller);
```

### 6.2 向后兼容策略

```cpp
class OrbitControls : public BaseControls, public WindowEventListener {
public:
    // ✅ 新接口
    bool onWindowEvent(const WindowEvent& event) override {
        handleWindowEvent(event);
        return false;
    }
    
    // ✅ 保留旧接口
    void handleWindowEvent(const WindowEvent& event) {
        // 现有代码不变
    }
};
```

### 6.3 迁移检查清单

- [ ] 继承`WindowEventListener`
- [ ] 实现`onWindowEvent()`
- [ ] 设置合适的优先级
- [ ] 使用`weak_ptr`存储监听器
- [ ] 析构时清理监听器
- [ ] 测试窗口关闭不崩溃
- [ ] 测试多监听器场景
- [ ] 验证事件传播控制

---

## 7. 性能与测试

### 7.1 性能指标

| 指标 | 数值 | 说明 |
|------|------|------|
| 时间复杂度 | O(n) | n为监听器数量 |
| 内存开销 | ~16字节/监听器 | weak_ptr |
| 线程安全 | ✅ | mutex保护 |
| 自动清理 | ✅ | 延迟清理 |

### 7.2 测试建议

#### 单元测试
```cpp
TEST(EventDispatcher, AddRemove) {
    auto dispatcher = std::make_unique<WindowEventDispatcher>();
    auto listener = std::make_shared<MockListener>();
    
    dispatcher->addEventListener(listener);
    EXPECT_EQ(dispatcher->getListenerCount(), 1);
    
    dispatcher->removeEventListener(listener);
    EXPECT_EQ(dispatcher->getListenerCount(), 0);
}
```

#### 集成测试
- ✅ Qt窗口关闭不崩溃
- ✅ 控制器切换（P键）
- ✅ 多监听器优先级
- ✅ 事件传播控制
- ✅ 生命周期管理

---

## 8. 编译问题解决

### 8.1 头文件引用错误

**问题**:
```
error C1083: 无法打开包括文件: "../../windowing/WindowEventSystem.h"
```

**原因**: 事件系统已集成到`Window.h`，不再有独立文件

**解决方案**:
```cpp
// ❌ 删除
#include "../../windowing/WindowEventSystem.h"

// ✅ 只需包含Window.h
#include "../../windowing/Window.h"
```

### 8.2 编译成功验证

✅ iengine.lib  
✅ EngineCoreTest.exe  
✅ GlfwAppExample.exe  
✅ QtAppExample.exe  

---

## 9. 总结

### 9.1 主要改进

| 方面 | 旧系统 | 新系统 |
|------|--------|--------|
| **生命周期安全** | ❌ 崩溃风险 | ✅ weak_ptr安全 |
| **多监听器** | ❌ 单一回调 | ✅ 无限监听器 |
| **优先级** | ❌ 无控制 | ✅ 精细排序 |
| **事件传播** | ❌ 不可控 | ✅ 可阻止传播 |
| **线程安全** | ❌ 不安全 | ✅ mutex保护 |

### 9.2 架构优势

- 符合SOLID原则
- 参考主流引擎设计（Unreal、Unity、Godot）
- 易于测试和维护
- 完善的生命周期管理

### 9.3 未来优化

- [ ] GLFW窗口完整支持
- [ ] 事件过滤器机制
- [ ] 性能优化（缓存、池化）
- [ ] 更多事件类型支持

---

**文档版本**: 1.0  
**最后更新**: 2025-10-13  
**维护者**: iEngine开发团队
