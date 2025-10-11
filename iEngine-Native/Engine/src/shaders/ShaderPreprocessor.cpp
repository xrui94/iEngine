#include "../../include/iengine/shaders/ShaderPreprocessor.h"

#include <regex>
#include <sstream>

namespace iengine {

    std::string ShaderPreprocessor::preprocessGlsl(
        const std::string& source,
        GraphicsAPI graphicsAPI,
        const std::string& type, // "vertex" or "fragment"
        const std::shared_ptr<DefineMap>& defines
    ) {
        // 1. 去除前导空白
        std::string code = source;
        code.erase(0, code.find_first_not_of(" \t\n\r"));
        
        // 2. 版本声明
        std::string header = "";
        if (graphicsAPI == GraphicsAPI::OpenGL) {
            // 对于 OpenGL，我们可能需要添加版本声明
            // 这里简化处理
        }
        
        // 3. 精度声明（简化处理）
        
        // 4. 宏定义
        if (defines && !defines->defines.empty()) {
            std::ostringstream macroStr;
            for (const auto& pair : defines->defines) {
                const std::string& key = pair.first;
                const std::string& value = pair.second;
                
                // 处理布尔类型
                if (value == "true") {
                    macroStr << "#define " << key << "\n";
                } else if (value != "false") { // false 不定义
                    macroStr << "#define " << key << " " << value << "\n";
                }
            }
            header += macroStr.str();
        }
        
        code = header + code;
        
        // 5. attribute/varying 替换（简化处理）
        if (graphicsAPI == GraphicsAPI::OpenGL) {
            // 在实际实现中，这里会进行相应的替换
            // 由于 C++ 版本使用 OpenGL，可能不需要这些替换
        }
        
        // 6. texture2D/texture 替换（简化处理）
        
        // 7. 片元着色器输出（简化处理）
        if (graphicsAPI == GraphicsAPI::OpenGL && type == "fragment") {
            // 在实际实现中，这里会处理片元着色器输出
        }
        
        return code;
    }

    std::string ShaderPreprocessor::preprocessWgsl(
        const std::string& wgsl,
        const std::shared_ptr<DefineMap>& defines
    ) {
        std::string processed = wgsl;
        
        // 先处理带代码块的宏
        if (defines) {
            for (const auto& pair : defines->defines) {
                const std::string& key = pair.first;
                const std::string& value = pair.second;
                
                // 构建块模式
                std::string blockPattern = "@define\\s+" + key + "\\s*\\{([\\s\\S]*?)\\}";
                std::regex blockRegex(blockPattern);
                
                // 替换块
                if (value != "false" && !value.empty()) {
                    processed = std::regex_replace(processed, blockRegex, "$1");
                } else {
                    processed = std::regex_replace(processed, blockRegex, "");
                }
            }
        }
        
        // 再处理单行宏标记
        std::regex singleLineRegex("@define\\s+\\w+.*\\n");
        processed = std::regex_replace(processed, singleLineRegex, "");
        
        return processed;
    }

} // namespace iengine