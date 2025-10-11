#include "../../include/iengine/shaders/ShaderLib.h"
#include "../../include/iengine/shaders/ShaderPreprocessor.h"

#include <algorithm>
#include <sstream>

namespace iengine {

    // 静态成员初始化
    std::unordered_map<std::string, std::shared_ptr<ShaderVariants>> ShaderLib::shaders_;
    std::unordered_map<ShaderVariantKey, std::shared_ptr<ShaderVariants>> ShaderLib::processedShaders_;

    void ShaderLib::registerShader(const std::string& name, std::shared_ptr<ShaderVariants> variants) {
        shaders_[name] = variants;
    }

    void ShaderLib::unregisterShader(const std::string& name) {
        shaders_.erase(name);
        
        // 清理所有相关变体缓存
        auto it = processedShaders_.begin();
        while (it != processedShaders_.end()) {
            if (it->first.find(name + "__") == 0) {
                it = processedShaders_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void ShaderLib::clearCache() {
        processedShaders_.clear();
    }

    std::shared_ptr<ShaderVariants> ShaderLib::getVariant(
        const std::string& name,
        GraphicsAPI backend,
        const ShaderVariantOptions& options
    ) {
        // 获取基础着色器
        auto shaderIt = shaders_.find(name);
        if (shaderIt == shaders_.end()) {
            return nullptr;
        }

        std::shared_ptr<ShaderVariants> shader = shaderIt->second;
        
        // 合并定义
        DefineMap mergedDefines;
        std::shared_ptr<DefineMap> baseDefines = nullptr;
        
        if (backend == GraphicsAPI::OpenGL) {
            if (shader->webgl && shader->webgl->defines) {
                baseDefines = shader->webgl->defines;
            }
        } else if (backend == GraphicsAPI::WebGPU) {
            if (shader->webgpu && shader->webgpu->defines) {
                baseDefines = shader->webgpu->defines;
            }
        }
        
        // 合并定义
        if (options.overrideDefaultDefines) {
            if (baseDefines) {
                mergedDefines.defines.insert(baseDefines->defines.begin(), baseDefines->defines.end());
            }
            if (options.defines) {
                mergedDefines.defines.insert(options.defines->defines.begin(), options.defines->defines.end());
            }
        } else {
            if (options.defines) {
                mergedDefines.defines.insert(options.defines->defines.begin(), options.defines->defines.end());
            }
            if (baseDefines) {
                mergedDefines.defines.insert(baseDefines->defines.begin(), baseDefines->defines.end());
            }
        }
        
        // 生成变体键
        ShaderVariantKey variantKey = makeShaderVariantKey(name, mergedDefines) + "__" + 
            (backend == GraphicsAPI::OpenGL ? "opengl" : "webgpu");
            
        // 检查缓存
        auto cachedIt = processedShaders_.find(variantKey);
        if (cachedIt != processedShaders_.end()) {
            return cachedIt->second;
        }
        
        // 处理着色器
        std::shared_ptr<ShaderVariants> result = std::make_shared<ShaderVariants>();
        
        if (backend == GraphicsAPI::OpenGL) {
            if (shader->webgl) {
                std::shared_ptr<GLSLSource> glsl = std::make_shared<GLSLSource>();
                glsl->vertCode = shader->webgl->vertCode.empty() ? "" : 
                    ShaderPreprocessor::preprocessGlsl(shader->webgl->vertCode, backend, "vertex", 
                                                      std::make_shared<DefineMap>(mergedDefines));
                glsl->fragCode = shader->webgl->fragCode.empty() ? "" : 
                    ShaderPreprocessor::preprocessGlsl(shader->webgl->fragCode, backend, "fragment", 
                                                      std::make_shared<DefineMap>(mergedDefines));
                glsl->defines = shader->webgl->defines;
                result->webgl = glsl;
            }
        } else if (backend == GraphicsAPI::WebGPU) {
            if (shader->webgpu) {
                std::shared_ptr<WGSLSource> wgsl = std::make_shared<WGSLSource>();
                if (!shader->webgpu->code.empty()) {
                    wgsl->code = ShaderPreprocessor::preprocessWgsl(shader->webgpu->code, 
                                                                   std::make_shared<DefineMap>(mergedDefines));
                } else {
                    wgsl->code = "";
                }
                wgsl->defines = shader->webgpu->defines;
                result->webgpu = wgsl;
            }
        }
        
        // 缓存结果
        processedShaders_[variantKey] = result;
        return result;
    }

    std::vector<std::string> ShaderLib::getAllShaderNames() {
        std::vector<std::string> names;
        names.reserve(shaders_.size());
        
        for (const auto& pair : shaders_) {
            names.push_back(pair.first);
        }
        
        return names;
    }

    void ShaderLib::registerBuiltInShaders() {
        // 在实际实现中，这里会注册内置着色器
        // 由于这是一个示例，我们只提供框架
    }

    ShaderVariantKey ShaderLib::makeShaderVariantKey(const std::string& shaderName, const DefineMap& defines) {
        if (defines.defines.empty()) {
            return shaderName;
        }
        
        // 创建排序后的定义字符串
        std::vector<std::pair<std::string, std::string>> sortedDefines(defines.defines.begin(), defines.defines.end());
        std::sort(sortedDefines.begin(), sortedDefines.end(), 
                 [](const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) {
                     return a.first < b.first;
                 });
        
        std::ostringstream oss;
        for (size_t i = 0; i < sortedDefines.size(); ++i) {
            if (i > 0) {
                oss << ";";
            }
            oss << sortedDefines[i].first << "=" << sortedDefines[i].second;
        }
        
        return shaderName + "__" + oss.str();
    }

} // namespace iengine