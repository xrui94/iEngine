#include "iengine/renderers/opengl/OpenGLRenderer.h"
#include "iengine/scenes/Scene.h"
#include "iengine/core/Model.h"
#include "iengine/core/Mesh.h"
#include "iengine/materials/Material.h"
#include "iengine/views/cameras/Camera.h"
#include "iengine/lights/Light.h"
#include "iengine/renderers/opengl/OpenGLContext.h"
#include "iengine/renderers/opengl/OpenGLShaderProgram.h"
#include "iengine/renderers/opengl/OpenGLRenderPipeline.h"
#include "iengine/shaders/ShaderLib.h"
#include "iengine/core/Enums.h"

#include <iostream>

namespace iengine {
    OpenGLRenderer::OpenGLRenderer() {}
    
    OpenGLRenderer::~OpenGLRenderer() {
        cleanup();
    }
    
    bool OpenGLRenderer::initialize(std::shared_ptr<Context> context) {
        // 初始化OpenGL渲染器
        m_openGLContext = std::dynamic_pointer_cast<OpenGLContext>(context);
        if (!m_openGLContext) {
            std::cerr << "OpenGLRenderer: Context not set" << std::endl;
            return false;
        }
        m_openGLContext->init();
        std::cout << "OpenGLRenderer initialized with context" << std::endl;
        return true;
    }
    
    void OpenGLRenderer::cleanup() {
        // 清理缓存的着色器和渲染管线
        shaders_.clear();
        renderPipelineCache_.clear();
    }
    
    void OpenGLRenderer::render(std::shared_ptr<Scene> scene) {
        currentCamera_ = scene->getActiveCamera();
        if (!currentCamera_) {
            std::cerr << "No active camera set for rendering" << std::endl;
            return;
        }
        
        // 获取场景中的所有组件（可渲染的对象）
        const auto& components = scene->getComponents();
        if (components.empty()) {
            std::cout << "No components to render in the scene" << std::endl;
            return;
        }
        
        // 获取场景中的所有光照
        const auto& lights = scene->getLights();
        
        // 清除画布
        clear();
        
        // 遍历所有组件，渲染每个组件
        for (const auto& component : components) {
            if (!component || !component->mesh) {
                std::cout << "Invalid component instance found!" << std::endl;
                continue;
            }
            
            // 1. 确保mesh顶点、索引等Buffer资源已经传到GPU
            if (!component->mesh->uploaded) {
                component->mesh->upload(m_openGLContext);
            }
            
            // 2. 根据材质特性，创建Shader
            std::map<std::string, bool> defines;
            // 合并mesh和material的宏定义
            auto meshDefines = component->mesh->getShaderMacroDefines();
            auto materialDefines = component->material->getShaderMacroDefines();
            defines.insert(meshDefines.begin(), meshDefines.end());
            defines.insert(materialDefines.begin(), materialDefines.end());
            
            // 调试输出：显示所有宏定义
            std::cout << "OpenGLRenderer: Shader defines for " << component->material->shaderName << ":" << std::endl;
            for (const auto& define : defines) {
                std::cout << "  " << define.first << " = " << (define.second ? "true" : "false") << std::endl;
            }
            
            // 获取或创建着色器
            auto shader = getOrCreateShader(component->material->shaderName, defines);
            if (!shader) {
                std::cerr << "Failed to get or create shader." << std::endl;
                continue;
            }
            
            // 3. 获取/创建 RenderPipeline
            auto pipeline = getOrCreatePipeline(component->mesh, shader);
            if (!pipeline) {
                std::cerr << "Failed to get or create render pipeline." << std::endl;
                continue;
            }
            
            // 4. 切换pipeline（切换着色器程序、绑定VAO、设置渲染状态）
            pipeline->bind();
            
            // 5. 设置uniform，将相机、材质、光照等参数数据绑定到Shader的uniform
            // 让材质/Shader自己决定需要哪些uniform
            auto uniforms = component->material->getUniforms(m_openGLContext, currentCamera_, component, lights);  // 传递 component（Model）而不是 mesh
            auto textures = component->material->getTextures();
            
            // 设置uniforms
            shader->setUniforms(uniforms);
            
            // 设置纹理（参照Web版本：texture在OpenGL中也是特殊的uniform）
            // 直接将纹理作为uniform传递给shader，与Web版本保持一致
            std::map<std::string, UniformValue> textureUniforms;
            for (const auto& texture : textures.textures) {
                textureUniforms[texture.first] = UniformValue(texture.second);
            }
            if (!textureUniforms.empty()) {
                shader->setUniforms(textureUniforms);
            }
            
            // 6. 绘制(DrawCall)
            m_openGLContext->draw(component->mesh);
            
            // 7. 解绑渲染管线（可选）
            pipeline->unbind();
        }
    }
    
    void OpenGLRenderer::resize(int width, int height) {
        if (m_openGLContext) {
            m_openGLContext->resize(width, height);
        }
        
        // 更新相机宽高比（如果是透视相机）
        if (currentCamera_) {
            // TODO: 检查相机类型并更新宽高比
            // if (auto perspectiveCamera = std::dynamic_pointer_cast<PerspectiveCamera>(currentCamera_)) {
            //     perspectiveCamera->setAspect(static_cast<float>(width) / height);
            // }
        }
    }
    
    void OpenGLRenderer::clear() {
        if (m_openGLContext) {
            m_openGLContext->clear();
        }
    }
    
    std::shared_ptr<OpenGLShaderProgram> OpenGLRenderer::getOrCreateShader(
        const std::string& shaderName,
        const std::map<std::string, bool>& defines) {
        
        // 生成着色器变体的键
        std::string key = shaderName;
        for (const auto& define : defines) {
            key += "_" + define.first + "_" + (define.second ? "1" : "0");
        }
        
        // 查找缓存中的着色器
        auto it = shaders_.find(key);
        if (it != shaders_.end()) {
            return it->second;
        }
        
        // 从ShaderLib获取着色器变体
        ShaderVariantOptions options;
        options.defines = std::make_shared<DefineMap>();
        for (const auto& define : defines) {
            options.defines->defines[define.first] = define.second ? "1" : "0";
        }
        
        auto variants = ShaderLib::getVariant(shaderName, GraphicsAPI::OpenGL, options);
        if (variants && variants->webgl && 
            !variants->webgl->vertCode.empty() && 
            !variants->webgl->fragCode.empty()) {
            auto shader = std::make_shared<OpenGLShaderProgram>(
                m_openGLContext, variants->webgl->vertCode, variants->webgl->fragCode);
            shaders_[key] = shader;
            return shader;
        }
        
        std::cerr << "Shader variant not found for: " << shaderName << std::endl;
        return nullptr;
    }
    
    std::shared_ptr<OpenGLRenderPipeline> OpenGLRenderer::getOrCreatePipeline(
        std::shared_ptr<Mesh> mesh, 
        std::shared_ptr<OpenGLShaderProgram> shader) {
        
        // 生成渲染管线的哈希键
        std::string key = makePipelineKey(mesh, shader);
        
        // 查找缓存中的渲染管线
        auto it = renderPipelineCache_.find(key);
        if (it != renderPipelineCache_.end()) {
            return it->second;
        }
        
        // 创建新的渲染管线
        auto pipeline = std::make_shared<OpenGLRenderPipeline>();
        
        // 设置 VAO 和顶点属性
        pipeline->setupVAO(mesh, shader, m_openGLContext);
        
        renderPipelineCache_[key] = pipeline;
        return pipeline;
    }
    
    std::string OpenGLRenderer::makePipelineKey(
        std::shared_ptr<Mesh> mesh, 
        std::shared_ptr<OpenGLShaderProgram> shader) {
        // 简单的哈希键生成，实际应该基于mesh和shader的特征
        return std::to_string(reinterpret_cast<uintptr_t>(mesh.get())) + "_" + 
               std::to_string(reinterpret_cast<uintptr_t>(shader.get()));
    }
}