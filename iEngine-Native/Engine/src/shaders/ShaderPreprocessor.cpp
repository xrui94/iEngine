#include "iengine/shaders/ShaderPreprocessor.h"

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
        
        // 2. 检查并提取现有的版本声明
        std::string versionLine = "";
        std::string codeWithoutVersion = code;
        size_t versionPos = code.find("#version");
        if (versionPos != std::string::npos) {
            // 提取版本声明行
            size_t lineEnd = code.find('\n', versionPos);
            if (lineEnd != std::string::npos) {
                versionLine = code.substr(versionPos, lineEnd - versionPos + 1);
                codeWithoutVersion = code.substr(lineEnd + 1);
            }
        }
        
        // 3. 构建头部声明（在版本声明之后）
        std::string header = "";
        
        // 3.1 如果没有版本声明，根据图形API添加
        if (versionLine.empty()) {
            if (graphicsAPI == GraphicsAPI::OpenGL) {
                versionLine = "#version 330 core\n";
            } else if (graphicsAPI == GraphicsAPI::WebGPU) {
                // WebGPU使用WGSL，不需要GLSL版本声明
            }
        }
        
        // 3.2 精度声明（片元着色器需要，且在版本声明之后）
        if (type == "fragment" && codeWithoutVersion.find("precision") == std::string::npos) {
            // 检查版本号，决定是否需要精度声明
            if (versionLine.find("330 core") != std::string::npos || 
                versionLine.find("410 core") != std::string::npos ||
                versionLine.find("450 core") != std::string::npos) {
                // OpenGL 3.3+ Core Profile 不需要精度声明，因为桌面OpenGL不支持precision修饰符
                // 跳过精度声明
            } else if (versionLine.find("300 es") != std::string::npos ||
                       versionLine.find("310 es") != std::string::npos) {
                // OpenGL ES 3.x 需要精度声明
                header += "precision mediump float;\n";
            } else {
                // WebGL 1.0 或其他情况，使用条件编译
                header += "#ifdef GL_ES\n";
                header += "precision mediump float;\n";
                header += "#endif\n";
            }
        }
        
        // 4. 宏定义（在版本声明之后）
        if (defines && !defines->defines.empty()) {
            for (const auto& pair : defines->defines) {
                const std::string& key = pair.first;
                const std::string& value = pair.second;
                
                // 处理布尔类型
                if (value == "true") {
                    header += "#define " + key + "\n";
                } else if (value != "false") { // false 不定义
                    header += "#define " + key + " " + value + "\n";
                }
            }
        }
        
        // 5. 组合代码：版本声明 + 头部 + 代码主体
        code = versionLine + header + codeWithoutVersion;
        
        // 5. attribute/varying 替换（OpenGL 3.3+需要）
        if (graphicsAPI == GraphicsAPI::OpenGL) {
            // 将传统的attribute、varying替换为现代OpenGL的in、out
            if (type == "vertex") {
                // 顶点着色器：attribute -> in, varying -> out
                code = std::regex_replace(code, std::regex("\\battribute\\b"), "in");
                code = std::regex_replace(code, std::regex("\\bvarying\\b"), "out");
            } else if (type == "fragment") {
                // 片元着色器：varying -> in
                code = std::regex_replace(code, std::regex("\\bvarying\\b"), "in");
                // 替换gl_FragColor
                if (code.find("gl_FragColor") != std::string::npos) {
                    // 在版本声明和精度声明之后添加输出声明
                    size_t insertPos = 0;
                    // 找到版本声明后的位置
                    if (code.find("#version") != std::string::npos) {
                        insertPos = code.find('\n', code.find("#version")) + 1;
                    }
                    // 跳过精度声明（如果存在）
                    if (code.find("precision", insertPos) != std::string::npos) {
                        size_t precisionPos = code.find("precision", insertPos);
                        insertPos = code.find('\n', precisionPos) + 1;
                    }
                    // 跳过宏定义
                    while (insertPos < code.length() && code.substr(insertPos, 7) == "#define") {
                        insertPos = code.find('\n', insertPos) + 1;
                    }
                    
                    // 插入输出声明
                    code.insert(insertPos, "out vec4 fragColor;\n");
                    code = std::regex_replace(code, std::regex("gl_FragColor"), "fragColor");
                }
            }
        }
        
        // 6. texture2D/texture 替换（OpenGL 3.3+使用texture）
        if (graphicsAPI == GraphicsAPI::OpenGL) {
            code = std::regex_replace(code, std::regex("\\btexture2D\\b"), "texture");
            code = std::regex_replace(code, std::regex("\\btextureCube\\b"), "texture");
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