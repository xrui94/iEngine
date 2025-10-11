#ifndef IENGINE_SHADER_LIB_H
#define IENGINE_SHADER_LIB_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "../core/Enums.h"

namespace iengine {

    // 前向声明
    class ShaderPreprocessor;
    
    struct DefineMap {
        std::unordered_map<std::string, std::string> defines;
    };

    struct ShaderVariantOptions {
        std::shared_ptr<DefineMap> defines;
        bool overrideDefaultDefines = true;
    };

    struct GLSLSource {
        std::string vertCode;
        std::string fragCode;
        std::shared_ptr<DefineMap> defines;
        // uniforms 信息在 C++ 版本中简化处理
    };

    struct WGSLSource {
        std::string code;
        std::shared_ptr<DefineMap> defines;
        // bindGroupEntryInfos 在 C++ 版本中简化处理
    };

    struct ShaderVariants {
        std::shared_ptr<GLSLSource> webgl;
        std::shared_ptr<WGSLSource> webgpu;
    };

    using ShaderVariantKey = std::string;

    class ShaderLib {
    public:
        // 注册着色器
        static void registerShader(const std::string& name, std::shared_ptr<ShaderVariants> variants);
        
        // 注销着色器
        static void unregisterShader(const std::string& name);
        
        // 清理缓存
        static void clearCache();
        
        // 获取着色器变体
        static std::shared_ptr<ShaderVariants> getVariant(
            const std::string& name,
            GraphicsAPI backend,
            const ShaderVariantOptions& options = ShaderVariantOptions()
        );
        
        // 获取所有着色器名称
        static std::vector<std::string> getAllShaderNames();
        
        // 注册内置着色器
        static void registerBuiltInShaders();

    private:
        // 着色器存储
        static std::unordered_map<std::string, std::shared_ptr<ShaderVariants>> shaders_;
        
        // 变体缓存
        static std::unordered_map<ShaderVariantKey, std::shared_ptr<ShaderVariants>> processedShaders_;
        
        // 生成着色器变体键
        static ShaderVariantKey makeShaderVariantKey(const std::string& shaderName, const DefineMap& defines);
    };

} // namespace iengine

#endif // IENGINE_SHADER_LIB_H