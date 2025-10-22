# iEngine

iEngine 是一个跨平台、跨语言的图形引擎，基于 C++17 标准。

## 🏗️ 架构约束

### Engine 实例管理

**推荐做法**（单 Engine 多 Scene）：

// ✅ 推荐：一个应用程序只创建一个 Engine 实例
```c++
iengine::Engine engine(options);

// 创建多个 Scene 来组织不同的渲染内容
auto mainScene = std::make_unique<iengine::Scene>();
auto uiScene = std::make_unique<iengine::Scene>();

engine.addScene("main", std::move(mainScene));
engine.addScene("ui", std::move(uiScene));
```

**为什么推荐单 Engine？**
- GPU 上下文开销大，多实例会浪费资源
- 全局资源（ShaderCache、TextureCache）共享更高效
- 简化资源管理和生命周期控制

**技术上是否强制？**
不强制，但会发出警告：
```text
[iengine] WARNING: Multiple Engine instances detected. 
This is discouraged due to GPU context overhead. 
Current count: 2
```

📝 TODO
[] 充分使用转移语义，明确 Engine、Scene 的及其持有的资源的生命周期管理
[] QtAppExample 示例程序，使用基于事件的渲染模式时，无法正常交互