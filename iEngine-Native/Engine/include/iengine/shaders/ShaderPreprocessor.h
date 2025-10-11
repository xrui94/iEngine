#ifndef IENGINE_SHADER_PREPROCESSOR_H
#define IENGINE_SHADER_PREPROCESSOR_H

#include <string>
#include <unordered_map>
#include <memory>

#include "ShaderLib.h"

namespace iengine {

    class ShaderPreprocessor {
    public:
        static std::string preprocessGlsl(
            const std::string& source,
            GraphicsAPI graphicsAPI,
            const std::string& type, // "vertex" or "fragment"
            const std::shared_ptr<DefineMap>& defines = nullptr
        );

        static std::string preprocessWgsl(
            const std::string& wgsl,
            const std::shared_ptr<DefineMap>& defines = nullptr
        );

    private:
        // 辅助函数
        static std::string processDefines(const std::shared_ptr<DefineMap>& defines);
    };

} // namespace iengine

#endif // IENGINE_SHADER_PREPROCESSOR_H