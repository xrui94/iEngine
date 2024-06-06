#include "ShaderModule.h"
#include "WebGPUDevice.h"

#include <cassert>
#include <iostream>
#include <chrono>	// Debug使用，使GetCompilationInfo的回调函数能够被执行
#include <thread>	// Debug使用，使GetCompilationInfo的回调函数能够被执行

namespace iEngine
{
	ShaderModule::ShaderModule(const fs::path& path)
		: m_ShaderModule(nullptr)
	{
		// std::cout << "Line 31: Shader from file:\n" << path << std::endl;
		std::ifstream file(path);
		if (!file.is_open()) {
			std::cerr << "Failed to open shader file: \"" << path.generic_string() << std::endl;
			return;
		}
		file.seekg(0, std::ios::end);
		size_t size = file.tellg();
		std::string shaderCode(size, ' ');
		file.seekg(0);
		file.read(shaderCode.data(), size);
		file.close();

		CreateShaderModule(shaderCode);
		// std::cout << "Shader module: " << m_ShaderModule << std::endl;	// EMSCRIPTEN下无法打印：invalid operands to binary expression
	}

	ShaderModule::ShaderModule(const std::string& shaderCode)
		: m_ShaderModule(nullptr)
	{
		// std::cout << "Line 31: Shader from code:\n" << shaderCode << std::endl;
		CreateShaderModule(shaderCode);
		// std::cout << "Shader module: " << m_ShaderModule << std::endl;	// EMSCRIPTEN下无法打印：invalid operands to binary expression
	}

	ShaderModule::~ShaderModule()
	{
	#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
		m_ShaderModule.release();
	#endif
	}

	void ShaderModule::CreateShaderModule(const std::string& shaderCode)
	{
		wgpu::ShaderModuleWGSLDescriptor shaderWgslDesc{};
		wgpu::ShaderModuleDescriptor shaderModuleDesc{};

		shaderWgslDesc.code = shaderCode.c_str();

	#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
		shaderWgslDesc.chain.next = nullptr;
		shaderWgslDesc.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;


		//
		shaderModuleDesc.nextInChain = &shaderWgslDesc.chain;

		#ifdef WEBGPU_BACKEND_WGPU
			shaderModuleDesc.hintCount = 0;
			shaderModuleDesc.hints = nullptr;
		#endif

		std::cout << "Line 67: Creating shader module..." << std::endl;
		m_ShaderModule = WebGPUDevice::Get().GetDevice().createShaderModule(shaderModuleDesc);
		#ifdef __EMSCRIPTEN__
			// WGPU-Native is not working.
			std::this_thread::sleep_for(std::chrono::seconds(1));
			m_ShaderModule.getCompilationInfo([](wgpu::CompilationInfoRequestStatus status, const wgpu::CompilationInfo& info) {
				assert(status == WGPUCompilationInfoRequestStatus_Success);
				assert(info.messageCount == 0);
				std::cout << "Shader compile succeeded." << std::endl;
			});
		#endif

	#elif defined(IE_ONLY_EMSCRIPTEN)

		std::cout << "Line 77: Create shader module " << std::endl;
		shaderModuleDesc.nextInChain = &shaderWgslDesc;
		m_ShaderModule = WebGPUDevice::Get().GetDevice().CreateShaderModule(&shaderModuleDesc);
		// 不加sleep，则无法触发GetCompilationInfo函数的回调函数，从而发现Shader中的错误
		// 同样，即使Shader中没有错误，GetCompilationInfo函数的回调函数，也不会被执行
		std::this_thread::sleep_for(std::chrono::seconds(1));
		m_ShaderModule.GetCompilationInfo([](WGPUCompilationInfoRequestStatus status, const WGPUCompilationInfo* info, void*) {
            assert(status == WGPUCompilationInfoRequestStatus_Success);
            assert(info->messageCount == 0);
            std::cout << "Shader compile succeeded." << std::endl;
        }, nullptr);

	#endif
	}
}
