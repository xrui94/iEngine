#pragma once

// 主要的引擎头文件，包含所有公共接口

// 核心类
#include "core/Engine.h"
#include "core/IRenderable.h"
#include "core/Mesh.h"
#include "core/Model.h"
#include "core/Primitive.h"

// 数学库
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Matrix2.h"
#include "math/Matrix3.h"
#include "math/Matrix4.h"

// 几何体
#include "geometries/Geometry.h"
#include "geometries/Cube.h"
#include "geometries/Triangle.h"

// 场景
#include "scenes/Scene.h"

// 相机
#include "views/cameras/Camera.h"
#include "views/cameras/PerspectiveCamera.h"
#include "views/cameras/OrthographicCamera.h"

// 控制器
#include "views/controls/BaseControls.h"
#include "views/controls/OrbitControls.h"
#include "views/controls/FirstPersonControls.h"

// 渲染器
#include "renderers/Renderer.h"
#include "renderers/opengl/OpenGLRenderer.h"
#include "renderers/opengl/OpenGLContext.h"
#include "renderers/opengl/OpenGLShaderProgram.h"
#include "renderers/opengl/OpenGLUniforms.h"
#include "renderers/opengl/OpenGLRenderPipeline.h"

// 上下文
#include "renderers/Context.h"

// 材质
#include "materials/Material.h"
#include "materials/BaseMaterial.h"
#include "materials/PhongMaterial.h"
#include "materials/PbrMaterial.h"
#include "materials/Color.h"

// 纹理
#include "textures/Texture.h"
#include "textures/Texture3D.h"
#include "textures/TextureUtils.h"

// 着色器
#include "shaders/ShaderLib.h"
#include "shaders/ShaderPreprocessor.h"

// 光源
#include "lights/Light.h"
#include "lights/AmbientLight.h"
#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"
#include "lights/SpotLight.h"

// 窗口系统（仅包含最基本的窗口抽象）
#include "windowing/Window.h"
#include "windowing/WindowFactory.h"