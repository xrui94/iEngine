#pragma once

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    #include <webgpu/webgpu.hpp>
#elif defined(IE_ONLY_EMSCRIPTEN)
    #include <webgpu/webgpu_cpp.h>
#endif

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace iEngine
{
    class ShaderModule
    {
    public:
        ShaderModule(const fs::path& path);

        ShaderModule(const std::string& shaderCode);

    ~ShaderModule();

    public:
        wgpu::ShaderModule GetShaderModule() { return m_ShaderModule; }

    private:
        void CreateShaderModule(const std::string& shaderCode);

    private:
        wgpu::ShaderModule m_ShaderModule;
    };
}
