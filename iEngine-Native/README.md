# iEngine Native (C++ Version)

A C++ implementation of the iEngine 3D rendering engine, designed to be compatible with the TypeScript/web version while leveraging native performance.

## Features

- Cross-platform C++17 implementation
- OpenGL rendering backend (Qt6/GLFW + glad)
- Planned WebGPU support via Dawn
- Modular architecture matching the TypeScript version
- Math library (Vector3, Matrix4)
- Scene management
- Camera system (PerspectiveCamera)
- Geometry primitives (Cube, Triangle)
- Material system (BaseMaterial)
- Lighting system
- Camera controls (OrbitControls, FirstPersonControls)

## Directory Structure

The C++ version follows the same directory structure as the TypeScript version:

```
iengine-native/
├── CMakeLists.txt              # 主CMake配置文件
├── README.md                   # 项目说明文档
├── build.bat                   # Windows构建脚本
├── include/iengine/            # 公共头文件
│   ├── core/                   # 核心组件
│   │   ├── Engine.h
│   │   ├── IRenderable.h
│   │   ├── Mesh.h
│   │   ├── Model.h
│   │   └── Primitive.h
│   ├── geometries/             # 几何体
│   │   ├── Geometry.h
│   │   ├── Cube.h
│   │   └── Triangle.h
│   ├── lights/                 # 光源系统
│   │   ├── Light.h
│   │   └── DirectionalLight.h
│   ├── materials/              # 材质系统
│   │   ├── Material.h
│   │   └── BaseMaterial.h
│   ├── math/                   # 数学库
│   │   ├── Vector3.h
│   │   └── Matrix4.h
│   ├── renderers/              # 渲染系统
│   │   ├── Renderer.h
│   │   ├── Context.h
│   │   └── opengl/             # OpenGL实现
│   │       ├── OpenGLRenderer.h
│   │       ├── OpenGLContext.h
│   │       ├── OpenGLShaderProgram.h
│   │       └── OpenGLUniforms.h
│   ├── scenes/                 # 场景管理
│   │   └── Scene.h
│   ├── views/                  # 视图系统
│   │   ├── cameras/            # 相机实现
│   │   │   ├── Camera.h
│   │   │   └── PerspectiveCamera.h
│   │   └── controls/           # 控制器实现
│   │       ├── BaseControls.h
│   │       ├── OrbitControls.h
│   │       └── FirstPersonControls.h
│   └── iengine.h               # 主要包含文件
└── src/                        # 源文件实现
    ├── core/                   # 核心组件实现
    │   └── Mesh.cpp
    ├── views/                  # 视图系统实现
    │   └── controls/           # 控制器实现
    │       ├── OrbitControls.cpp
    │       └── FirstPersonControls.cpp
    ├── renderers/              # 渲染系统实现
    │   └── opengl/             # OpenGL实现
    │       ├── OpenGLUniforms.cpp
    │       └── OpenGLShaderProgram.cpp
    └── main.cpp                # 示例应用程序
```

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- Qt6 (Core, Widgets, OpenGL, OpenGLWidgets)
- Optional: GLFW3 (for alternative OpenGL context creation)

### Build Steps

1. Clone the repository
2. Create a build directory:
   ```
   mkdir build
   cd build
   ```
3. Configure with CMake:
   ```
   cmake ..
   ```
4. Build the project:
   ```
   cmake --build .
   ```

## Usage

The engine provides a simple API similar to the TypeScript version:

```cpp
#include "iengine/iengine.h"

// Create engine instance
iengine::EngineOptions options;
options.renderer = iengine::RendererType::OpenGL;
iengine::Engine engine(options);

// Create scene
auto scene = std::make_shared<iengine::Scene>();

// Create camera
auto camera = std::make_shared<iengine::PerspectiveCamera>(60.0f, 1.0f, 0.1f, 100.0f);
scene->setActiveCamera(camera);

// Create geometry, mesh, material and model
auto geometry = std::make_shared<iengine::Cube>(1.0f);
auto primitive = std::make_shared<iengine::Primitive>(iengine::PrimitiveType::TRIANGLES);
auto mesh = std::make_shared<iengine::Mesh>(geometry, primitive);

iengine::BaseMaterialParams params;
params.color = iengine::Color(1.0f, 0.0f, 0.0f, 1.0f); // Red
auto material = std::make_shared<iengine::BaseMaterial>(params);

auto model = std::make_shared<iengine::Model>("Cube", mesh, material);

// Add to scene and engine
scene->addComponent(model);
engine.addScene("main", scene);
engine.setActiveScene("main");

// Run the engine
engine.start();
```

## Architecture

The engine follows a component-based architecture:

- **Engine**: Main engine class that manages scenes and rendering
- **Scene**: Container for models, lights, and cameras
- **Camera**: Viewpoint for rendering (PerspectiveCamera implementation)
- **Model**: Renderable object with mesh and material
- **Mesh**: Geometry data (vertices, indices) and primitive type
- **Geometry**: Raw geometric data (Cube, Triangle implementations)
- **Material**: Surface properties (BaseMaterial implementation)
- **Renderer**: Rendering backend (OpenGLRenderer implementation)
- **Context**: Graphics context abstraction (OpenGLContext implementation)
- **Controls**: Camera controllers (OrbitControls, FirstPersonControls)

## Future Work

- Implement complete OpenGL rendering pipeline
- Add WebGPU support via Dawn
- Implement more geometry primitives
- Add advanced materials (PBR, etc.)
- Implement lighting systems (PointLight, SpotLight, etc.)
- Add texture support
- Implement animation system
- Add post-processing effects

## License

This project is licensed under the MIT License - see the LICENSE file for details.