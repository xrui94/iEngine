#pragma once

#include "../../windows/Window.h"

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    #include <webgpu/webgpu.hpp>
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
    #include <webgpu/webgpu_cpp.h>
#endif

#include "../../VertexAttrs.h"
#include "../../Mesh.h"

#ifdef IE_GLFW_WINDOW
	#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE)
		#include <glfw3webgpu.h>
	#endif
	#include <GLFW/glfw3.h>
#elif defined(IE_HTML_WINDOW)
	// #include "HtmlWindow.h"
	class HtmlWindow;
#endif

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <memory>
#include <optional>
#include <iostream>

struct UniformLayoutEntryInfo
{
	uint32_t				binding;
#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	wgpu::ShaderStageFlags		visibility;
#elif defined(IE_ONLY_EMSCRIPTEN)
	wgpu::ShaderStage		visibility;
#endif
	uint64_t				minBindingSize;
};

struct TextureLayoutEntryInfo
{
	uint32_t					binding;
	wgpu::ShaderStage			visibility;
	wgpu::TextureSampleType		sampleType;
	wgpu::TextureViewDimension	viewDimension;
	bool						multisampled;
};

struct StorageTextureLayoutEntryInfo
{
	uint32_t					binding;
	wgpu::ShaderStage			visibility;
	wgpu::StorageTextureAccess	access;
	wgpu::TextureFormat			format;
	wgpu::TextureViewDimension	viewDimension;
};

struct SamplerLayoutEntryInfo
{
	uint32_t					binding;
	wgpu::ShaderStage			visibility;
	wgpu::SamplerBindingType	type;
};


//
struct UniformEntryInfo
{
	uint32_t		binding;
	wgpu::Buffer	buffer;
	uint64_t		offset;
	uint64_t		size;
};

struct TextureEntryInfo
{
	uint32_t			binding;
	wgpu::TextureView	textureView;
};

struct StorageTextureEntryInfo
{
	uint32_t		binding;
	// wgpu::StorageTextureView	access;
	// wgpu::TextureFormat			format;
	// wgpu::TextureViewDimension	viewDimension;
};

struct SamplerEntryInfo
{
	uint32_t		binding;
	wgpu::Sampler	sampler;
};

struct Frame
{
	wgpu::CommandEncoder encoder;
	wgpu::RenderPassEncoder renderPass;
	wgpu::TextureView textureView;
};


class WebGPUDevice
{
private:
	// 禁止外部构造
    WebGPUDevice();

    // 禁止外部析构
    ~WebGPUDevice();

    // 禁止外部拷贝构造
    WebGPUDevice(const WebGPUDevice& webGPUDevice) = delete;

    // 禁止外部赋值操作
    const WebGPUDevice& operator=(const WebGPUDevice& webGPUDevice) = delete;

public:
	static WebGPUDevice& Get();

	void Destroy();

	bool IsInitialized() const { return m_IsInitialized; }

#if defined(IE_ONLY_EMSCRIPTEN)
	using AdapterAndDeviceCallback = std::function<void(wgpu::Device)>;

	void RequestAdapterAndDevice(AdapterAndDeviceCallback callback);
#endif

	wgpu::Device GetDevice() const { 
		return m_Device;
	}

    bool InitDevice(Window* window);

	void DestroyDevice();

	void CreareSurface(/*Window* window*/);

	bool CreateSwapChain(uint32_t width, uint32_t height);

	void DestroySwapChain();

	float GetTextureFormatGamma(wgpu::TextureFormat format = wgpu::TextureFormat::Undefined) const;

	// Vertex Attributions and It's Indices
    wgpu::VertexBufferLayout CreateVertexBufferLayout(std::vector<wgpu::VertexAttribute>& vertexAttrs);

    wgpu::Buffer CreateVertexBuffer(std::vector<VertexAttrs>& vertexAttrs, const std::string& label = "");

	wgpu::Buffer CreateVertexBuffer(std::vector<float>& vertexAttrs, const std::string& label = "");

    void DestroyVertexBuffer();

	// template<typename T>
	// wgpu::Buffer CreateIndexBuffer(std::vector<T> indexData, std::string& name)
	// {
	// 	CreateIndexBufferImpl
	// }

    // wgpu::Buffer CreateIndexBufferImpl();

	wgpu::Buffer CreateIndexBuffer(std::vector<uint32_t> indexData, const std::string& label = "");

    void DestroyIndexBuffer();

	// Uniform
	template<typename UniformDecl>
    wgpu::Buffer CreateUniformBuffer(const std::string& name = "")
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = name.c_str();
		bufferDesc.size = sizeof(UniformDecl);
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
		bufferDesc.mappedAtCreation = false;
	#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
		return m_Device.createBuffer(bufferDesc);
	#elif defined(IE_ONLY_EMSCRIPTEN)
		return m_Device.CreateBuffer(&bufferDesc);
	#endif
	}

	void WriteUniformBuffer(wgpu::Buffer buffer, size_t offset, void const* data, size_t size);

	void DestroyUniformBuffer();

	// Material, Texture and Sampler
    void CreateTexture();

	wgpu::Texture CreateDepthTexture();

	wgpu::TextureView CreateDepthTextureView(wgpu::Texture depthTexture);

    void CreateSampler();

	// Bind Group Layout
    void CreateUniformBindGroupLayoutEntries(std::vector<UniformLayoutEntryInfo>& uniformInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries);

	void CreateTextureBindGroupLayoutEntries(std::vector<TextureLayoutEntryInfo>& textureInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries);

	void CreateStorageTextureBindGroupLayoutEntries(std::vector<StorageTextureLayoutEntryInfo>& storageTextureInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries);

	void CreateSamplerBindGroupLayoutEntries(std::vector<SamplerLayoutEntryInfo>& samplerInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries);

	wgpu::BindGroupLayout CreateBindGroupLayout(std::vector<wgpu::BindGroupLayoutEntry>& entries);

	// Bind Group
	void CreateUniformBindGroupEntry(std::vector<UniformEntryInfo>& uniformInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries);

	void CreateTextureBindGroupEntry(std::vector<TextureEntryInfo>& uniformInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries);

	void CreateStorageTextureBindGroupEntry(std::vector<StorageTextureEntryInfo>& uniformInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries);

	void CreateSamplerBindGroupEntry(std::vector<SamplerEntryInfo>& uniformInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries);

	wgpu::BindGroup CreateBindGroup(std::vector<wgpu::BindGroupEntry>& entries, wgpu::BindGroupLayout bindGroupLayout, const std::string& label = "");

	// State
	wgpu::VertexState CreateVertState(wgpu::ShaderModule& shaderModule) const;

	wgpu::BlendState CreateBlendState() const;

	wgpu::DepthStencilState CreateDepthStencilState() const;

	void CreateColorState(wgpu::ColorTargetState& colorTarget, wgpu::BlendState& blendState) const;

	void CreateFragState(wgpu::FragmentState& fragState, wgpu::ShaderModule& shaderModule, wgpu::ColorTargetState& colorTarget) const;

	// Render Pipeline
    wgpu::PipelineLayout CreateRenderPipelineLayout(std::vector<wgpu::BindGroupLayout> bindGroupLayouts);

    wgpu::RenderPipeline CreateRenderPipeline(
		wgpu::VertexBufferLayout& vertexBufferLayout,
		wgpu::BlendState& blendState,
		wgpu::PipelineLayout& pipelineLayout,
		wgpu::ShaderModule shaderModule);

	// Draw
    // void Draw(std::shared_ptr<Mesh> mesh, glm::vec4& clearColor);

	std::optional<Frame> Begin(glm::vec4& clearColor);

	void Draw(Frame& frame, std::shared_ptr<Mesh> mesh);

	void End(Frame& frame);

// private:
//     void CreareSurface(Window* window);

private:
	bool m_IsInitialized;

	std::unique_ptr<wgpu::DeviceLostCallback> m_DeviceLostCallbackHandle;

	std::unique_ptr<wgpu::ErrorCallback> m_ErrorCallbackHandle;

	uint32_t m_Width;
	uint32_t m_Height;

  	static WebGPUDevice* s_Instance;

    wgpu::Instance m_Instance;

	Window* m_Window;

	wgpu::Surface m_Surface;

	wgpu::Device m_Device;

	wgpu::Queue m_Queue;

	// Keep the error callback alive
	std::unique_ptr<wgpu::ErrorCallback> m_errorCallbackHandle;

	wgpu::TextureFormat m_SwapChainFormat;

	// Swap Chain
	wgpu::SwapChain m_SwapChain;

	// // Depth Buffer
	// wgpu::TextureFormat m_depthTextureFormat = wgpu::TextureFormat::Depth24Plus;
	// wgpu::Texture m_depthTexture = nullptr;
	// wgpu::TextureView m_depthTextureView = nullptr;

	// Shader
	//wgpu::ShaderModule m_ShaderModule;

    // Render Pipeline
	std::vector<wgpu::RenderPipeline> m_Pipelines;

	// // Texture
	// wgpu::Sampler m_sampler = nullptr;
	// wgpu::Texture m_texture = nullptr;
	// wgpu::TextureView m_textureView = nullptr;

	// // Uniforms
	// wgpu::Buffer m_uniformBuffer = nullptr;
	// GlobalUniforms m_uniforms;
	// wgpu::Buffer m_lightingUniformBuffer = nullptr;
	// LightingUniforms m_lightingUniforms;
	// bool m_lightingUniformsChanged = true;

	// Bind Group Layout
	wgpu::BindGroupLayout m_BindGroupLayout = nullptr;
	// wgpu::BindGroupLayout m_materialBindGroupLayout = nullptr;
	// wgpu::BindGroupLayout m_nodeBindGroupLayout = nullptr;

	// Bind Group
	wgpu::BindGroup m_BindGroup = nullptr;

    std::vector<wgpu::Buffer> m_Buffers;

    std::vector<wgpu::Buffer> m_IndexBuffers;
};