#include "WebGPUDevice.h"

#ifdef IE_HTML_WINDOW
	#include "windows/HtmlWindow.h"
#endif

#include <iostream>
#include <map>

WebGPUDevice* WebGPUDevice::s_Instance = nullptr;

WebGPUDevice::WebGPUDevice()
	: m_Width(0), m_Height(0), m_Window{ nullptr },
	m_Instance(nullptr), m_Surface(nullptr), m_Device(nullptr), m_Queue(nullptr),
	m_SwapChain(nullptr), m_SwapChainFormat(wgpu::TextureFormat::Undefined), m_IsInitialized(false)
{
}

WebGPUDevice::~WebGPUDevice()
{
	// vertexBuffer.destroy();
	// vertexBuffer.release();

	// // Destroy the depth texture and its view
	// depthTextureView.release();
	// depthTexture.destroy();
	// depthTexture.release();

	// pipeline.release();
	// shaderModule.release();

	delete s_Instance;
}

void WebGPUDevice::Destroy()
{
	// 注意顺序
	DestroySwapChain();
	DestroyDevice();
}

WebGPUDevice& WebGPUDevice::Get()
{
	if (!s_Instance) s_Instance = new WebGPUDevice;
	return *s_Instance;
}

#if defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	void WebGPUDevice::RequestAdapterAndDevice(AdapterAndDeviceCallback callback)
	{
		auto requestAdapterCallback = [](WGPURequestAdapterStatus status, WGPUAdapter cAdapter, const char *message, void *userdata)
		{
			if (status != WGPURequestAdapterStatus_Success)
			{
				std::cout << "Failed to request adapter, Error: " << message << std::endl;
				exit(0);
			}

			auto requestDeviceCallback = [](WGPURequestDeviceStatus status, WGPUDevice cDevice, const char *message, void *userdata)
			{
				if (status != WGPURequestAdapterStatus_Success)
				{
					std::cout << "Failed to request device, Error: " << message << std::endl;
					exit(0);
				}

				wgpu::Device device = wgpu::Device::Acquire(cDevice);
				AdapterAndDeviceCallback callback = *reinterpret_cast<AdapterAndDeviceCallback*>(userdata);
				callback(device);
			};

			//
			wgpu::Adapter adapter = wgpu::Adapter::Acquire(cAdapter);
			adapter.RequestDevice(nullptr, requestDeviceCallback, userdata);
		};

		wgpu::RequestAdapterOptions adapterOpts{};
		#ifdef IE_DAWN_NATIVE
			adapterOpts.backendType = wgpu::BackendType::Vulkan;
		#endif
		m_Instance.RequestAdapter(&adapterOpts, requestAdapterCallback, new AdapterAndDeviceCallback(callback));
	}
#endif

bool WebGPUDevice::InitDevice(Window* window)
{
	m_Window = window;

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	#ifdef __EMSCRIPTEN__
		// m_Instance = wgpu::createInstance(nullptr);	// emsdk-v3.1.61, It's not working.
		m_Instance = wgpuCreateInstance(nullptr);	// emsdk-v3.1.61, working
	#else
		// wgpu::InstanceDescriptor instanceDesc = wgpu::Default;
		// m_Instance = wgpu::createInstance(instanceDesc);
		m_Instance = wgpu::createInstance(wgpu::InstanceDescriptor{});
	#endif
	if (!m_Instance) {
		std::cerr << "Failed to create instance for webgpu on initialization." << std::endl;
		return false;
	}

	std::cout << "Creating surface..." << std::endl;
	CreareSurface(/*window*/);
	if (!m_Surface)
	{
		std::cerr << "Failed to create surface for webgpu on initialization." << std::endl;
		return false;
	}

    //
	std::cout << "Requesting adapter..." << std::endl;
	wgpu::RequestAdapterOptions adapterOpts{};
	adapterOpts.compatibleSurface = m_Surface;
	wgpu::Adapter adapter = m_Instance.requestAdapter(adapterOpts);	// async
	std::cout << "Got adapter: " << adapter << std::endl;

	//
	#ifdef WEBGPU_BACKEND_WGPU
		m_SwapChainFormat = m_Surface.getPreferredFormat(adapter);
	#else
		m_SwapChainFormat = wgpu::TextureFormat::BGRA8Unorm;
	#endif	// WEBGPU_BACKEND_WGPU

	//
	wgpu::SupportedLimits supportedLimits;
	
	#ifdef __EMSCRIPTEN__
		// Error in Chrome: Aborted(TODO: wgpuAdapterGetLimits unimplemented)
		// (as of September 4, 2023), so we hardcode values:
		// These work for 99.95% of clients (source: https://web3dsurvey.com/webgpu)
		supportedLimits.limits.minStorageBufferOffsetAlignment = 256;
		supportedLimits.limits.minUniformBufferOffsetAlignment = 256;
	#else
		adapter.getLimits(&supportedLimits);
	#endif

	std::cout << "Requesting device..." << std::endl;
	wgpu::RequiredLimits requiredLimits = wgpu::Default;
	requiredLimits.limits.maxVertexAttributes = 4;
	requiredLimits.limits.maxVertexBuffers = 4;
	requiredLimits.limits.maxBufferSize = 1500000 * sizeof(VertexAttrs);
	requiredLimits.limits.maxVertexBufferArrayStride = sizeof(VertexAttrs);
	requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;
	requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
	requiredLimits.limits.maxInterStageShaderComponents = 11;
	requiredLimits.limits.maxBindGroups = 3;
	requiredLimits.limits.maxUniformBuffersPerShaderStage = 2;
	requiredLimits.limits.maxUniformBufferBindingSize = static_cast<uint64_t>(16) * 4 * sizeof(float);
	// Allow textures up to 2K
	requiredLimits.limits.maxTextureDimension1D = 2048;
	requiredLimits.limits.maxTextureDimension2D = 2048;
	requiredLimits.limits.maxTextureArrayLayers = 1;
	requiredLimits.limits.maxSampledTexturesPerShaderStage = 3;
	requiredLimits.limits.maxSamplersPerShaderStage = 3;

    //
	wgpu::DeviceDescriptor deviceDesc;
	deviceDesc.label = "My Device";
	#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE)
		deviceDesc.requiredFeaturesCount = 0; // webgpu-native可以跑通？？？!!!!!!!!!!!!!!!!!!!!!!!!!! 
	#elif defined(IE_WGPU_EMSCRIPTEN)
		deviceDesc.requiredFeatureCount = 0;	// emsdk-v3.1.61
	#endif
	// deviceDesc.requiredFeaturesCount = 0;	// 3.1.47 + 引用webgpu-cpp仓库可用，3.1.57 + 手动拷贝的webgpu.hpp/仓库都不可用
	deviceDesc.requiredLimits = &requiredLimits;
	deviceDesc.defaultQueue.label = "The default queue";
	m_Device = adapter.requestDevice(deviceDesc);	// async
	std::cout << "Got device: " << m_Device << std::endl;

	//
	m_DeviceLostCallbackHandle = m_Device.setDeviceLostCallback([](wgpu::DeviceLostReason reason, char const* message) {
		std::cout << "Device lost: Reason: " << (int)reason;
		if (message) std::cout << ", Message: " << message << "." << std::endl;
	});

	// Add an error callback for more debug info
	m_ErrorCallbackHandle = m_Device.setUncapturedErrorCallback([](wgpu::ErrorType type, char const* message) {
		std::cout << "Device error: Type: " << type;
		if (message) std::cout << ", message: " << message << "." << std::endl;
	});

	//
	m_Queue = m_Device.getQueue();

	//
	adapter.release();

	// swap chain
	CreateSwapChain(window->GetWidth(), window->GetHeight());

#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)

	//
    m_Instance = wgpu::CreateInstance();
    if (m_Instance == nullptr)
    {
        std::cerr << "Failed to create instance for webgpu on initialization." << std::endl;
        return false;
    }

    //
    CreareSurface(/*window*/);
    if (m_Surface == nullptr)
    {
        std::cerr << "Failed to create surface for webgpu on initialization." << std::endl;
        return false;
    }

    //
    m_SwapChainFormat = wgpu::TextureFormat::BGRA8Unorm;

    //
    AdapterAndDeviceCallback callback = [this, &window](wgpu::Device device){
        //
        m_Device = device;
		m_Device.SetUncapturedErrorCallback(
			[](WGPUErrorType type, const char *message, void */*userdata*/)
			{
				std::cout << "Error: " << type << " , message: " << message;
			},
			nullptr
		);

        //
        m_Queue = m_Device.GetQueue();

        //
        CreateSwapChain(window->GetWidth(), window->GetHeight());

        //
        m_IsInitialized = true;
    };
    RequestAdapterAndDevice(callback);

    // Do loop for awaiting initialization（Cyclic execution, affecting performance？？？）
    // However, it will return a promise immediately without "await"!
	#ifdef __EMSCRIPTEN__
		while (!m_IsInitialized)
		{
			emscripten_sleep(100);
		}
	#endif	// __EMSCRIPTEN__

#endif // IE_WGPU_NATIVE || IE_WGPU_EMSCRIPTEN

	//
	if (m_Device != nullptr && m_SwapChain != nullptr)
	{
		m_IsInitialized = true;
	}

	return m_IsInitialized;
}

void WebGPUDevice::DestroyDevice()
{
	// 注意顺序
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	m_Queue.release();
	m_Device.release();
	m_Surface.release();
	m_Instance.release();
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	m_Device.Destroy();
#endif
}

void WebGPUDevice::CreareSurface(/*Window* window*/)
{
#if defined(__EMSCRIPTEN__)
    wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
	HtmlWindow* htmlWindow = static_cast<HtmlWindow*>(m_Window->GetNativeWindow());
    canvasDesc.selector = std::string("#").append(htmlWindow->GetCanvasId()).c_str();

    #if defined(IE_WGPU_EMSCRIPTEN)
        wgpu::SurfaceDescriptor surfaceDesc{};

        canvasDesc.chain.next = nullptr;
        canvasDesc.chain.sType = wgpu::SType::SurfaceDescriptorFromCanvasHTMLSelector;
        // if (canvasDesc.chain.next == nullptr) std::cout << "===================" << std::endl;

        surfaceDesc.label = "html5 canvas";
        surfaceDesc.nextInChain = &canvasDesc.chain;
        m_Surface = m_Instance.createSurface(surfaceDesc);  // not working
    #elif defined(IE_ONLY_EMSCRIPTEN)
        wgpu::SurfaceDescriptor surfaceDesc{
            .nextInChain = &canvasDesc
        };
        m_Surface = m_Instance.CreateSurface(&surfaceDesc);
    #endif
#else
	#if defined(IE_DAWN_NATIVE)
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
		m_Surface = wgpu::glfw::CreateSurfaceForWindow(m_Instance, glfwWindow);
	#elif defined(IE_WGPU_NATIVE)
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
		m_Surface = glfwGetWGPUSurface(m_Instance, glfwWindow);
	#endif
#endif	// __EMSCRIPTEN__
}

bool WebGPUDevice::CreateSwapChain(uint32_t width, uint32_t height)
{
	m_Width = width;
	m_Height = height;

	//
	// std::cout << "Current m_SwapChain..." << m_SwapChain << std::endl;
	std::cout << "Creating swapchain..." << std::endl;
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	//wgpu::SurfaceTexture surfaceTexture;
	//m_Surface.getCurrentTexture(&surfaceTexture);
	//surfaceTexture.Drop();

	wgpu::SwapChainDescriptor swapChainDesc;
	swapChainDesc.width = width;
	swapChainDesc.height = height;
	swapChainDesc.usage = wgpu::TextureUsage::RenderAttachment;
	swapChainDesc.format = m_SwapChainFormat;
	swapChainDesc.presentMode = wgpu::PresentMode::Fifo;
	m_SwapChain = m_Device.createSwapChain(m_Surface, swapChainDesc);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	wgpu::SwapChainDescriptor swapChainDesc{};
	swapChainDesc.usage = wgpu::TextureUsage::RenderAttachment;
	swapChainDesc.format = m_SwapChainFormat;
	swapChainDesc.width = width;
	swapChainDesc.height = height;
	swapChainDesc.presentMode = wgpu::PresentMode::Fifo;
    m_SwapChain = m_Device.CreateSwapChain(m_Surface, &swapChainDesc);
#endif	// IE_WGPU_NATIVE || IE_WGPU_EMSCRIPTEN

	// std::cout << "Swapchain: " << m_SwapChain << std::endl;	// EMSCRIPTEN环境下无法打印：invalid operands to binary expression

	return true;
}

void WebGPUDevice::DestroySwapChain()
{
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	m_SwapChain.release();
#endif
	m_SwapChain = nullptr;
}

float WebGPUDevice::GetTextureFormatGamma(wgpu::TextureFormat format) const
{
	if (format == wgpu::TextureFormat::Undefined) format = m_SwapChainFormat;

	switch (format)
	{
	case wgpu::TextureFormat::ASTC10x10UnormSrgb:
	case wgpu::TextureFormat::ASTC10x5UnormSrgb:
	case wgpu::TextureFormat::ASTC10x6UnormSrgb:
	case wgpu::TextureFormat::ASTC10x8UnormSrgb:
	case wgpu::TextureFormat::ASTC12x10UnormSrgb:
	case wgpu::TextureFormat::ASTC12x12UnormSrgb:
	case wgpu::TextureFormat::ASTC4x4UnormSrgb:
	case wgpu::TextureFormat::ASTC5x5UnormSrgb:
	case wgpu::TextureFormat::ASTC6x5UnormSrgb:
	case wgpu::TextureFormat::ASTC6x6UnormSrgb:
	case wgpu::TextureFormat::ASTC8x5UnormSrgb:
	case wgpu::TextureFormat::ASTC8x6UnormSrgb:
	case wgpu::TextureFormat::ASTC8x8UnormSrgb:
	case wgpu::TextureFormat::BC1RGBAUnormSrgb:
	case wgpu::TextureFormat::BC2RGBAUnormSrgb:
	case wgpu::TextureFormat::BC3RGBAUnormSrgb:
	case wgpu::TextureFormat::BC7RGBAUnormSrgb:
	case wgpu::TextureFormat::BGRA8UnormSrgb:
	case wgpu::TextureFormat::ETC2RGB8A1UnormSrgb:
	case wgpu::TextureFormat::ETC2RGB8UnormSrgb:
	case wgpu::TextureFormat::ETC2RGBA8UnormSrgb:
	case wgpu::TextureFormat::RGBA8UnormSrgb:
		return 2.2f;
	default:
		return 1.0f;
	}
}

wgpu::VertexBufferLayout WebGPUDevice::CreateVertexBufferLayout(std::vector<wgpu::VertexAttribute>& vertexAttrs)
{
	wgpu::VertexBufferLayout vertexBufferLayout;
	vertexBufferLayout.attributeCount = (uint32_t)vertexAttrs.size();
	vertexBufferLayout.attributes = vertexAttrs.data();
	vertexBufferLayout.arrayStride = sizeof(VertexAttrs);
	//                               ^^^^^^^^^^^^^^^^^^^^^^^^ This was 6 * sizeof(float)
	vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

	return vertexBufferLayout;
}

wgpu::Buffer WebGPUDevice::CreateVertexBuffer(std::vector<VertexAttrs>& vertexAttrs, const std::string& label)
{
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    wgpu::BufferDescriptor bufferDesc = wgpu::Default;
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	wgpu::BufferDescriptor bufferDesc{};
#endif
    bufferDesc.label = label.c_str();
    // bufferDesc.size = alignToNextMultipleOf(static_cast<uint32_t>(buffer.data.size()), 4);
	bufferDesc.size = vertexAttrs.size() * sizeof(VertexAttrs);
    // bufferDesc.usage = BufferUsage::CopyDst | BufferUsage::Vertex | BufferUsage::Index;
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
	bufferDesc.mappedAtCreation = false;

	//
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    wgpu::Buffer vertexBuffer = m_Device.createBuffer(bufferDesc);
    m_Queue.writeBuffer(vertexBuffer, 0, vertexAttrs.data(), bufferDesc.size);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
    wgpu::Buffer vertexBuffer = m_Device.CreateBuffer(&bufferDesc);
    m_Queue.WriteBuffer(vertexBuffer, 0, vertexAttrs.data(), bufferDesc.size);
#endif
    //
    return vertexBuffer;
}

wgpu::Buffer WebGPUDevice::CreateVertexBuffer(std::vector<float>& vertexAttrs, const std::string& label)
{
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    wgpu::BufferDescriptor bufferDesc = wgpu::Default;
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	wgpu::BufferDescriptor bufferDesc{};
#endif
    bufferDesc.label = label.c_str();
    // bufferDesc.size = alignToNextMultipleOf(static_cast<uint32_t>(buffer.data.size()), 4);
	bufferDesc.size = vertexAttrs.size() * sizeof(float);
    // bufferDesc.usage = BufferUsage::CopyDst | BufferUsage::Vertex | BufferUsage::Index;
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
	bufferDesc.mappedAtCreation = false;

	//
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    wgpu::Buffer vertexBuffer = m_Device.createBuffer(bufferDesc);
    m_Queue.writeBuffer(vertexBuffer, 0, vertexAttrs.data(), bufferDesc.size);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
    wgpu::Buffer vertexBuffer = m_Device.CreateBuffer(&bufferDesc);
    m_Queue.WriteBuffer(vertexBuffer, 0, vertexAttrs.data(), bufferDesc.size);
#endif
    //
    return vertexBuffer;
}

wgpu::Buffer WebGPUDevice::CreateIndexBuffer(std::vector<uint32_t> indexData, const std::string& label)
{
    // Create index buffer
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	wgpu::BufferDescriptor bufferDesc = wgpu::Default;
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	wgpu::BufferDescriptor bufferDesc{};
#endif
	bufferDesc.label = label.c_str();
	bufferDesc.size = indexData.size() * sizeof(uint32_t);
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
	bufferDesc.mappedAtCreation = false;

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	wgpu::Buffer indexBuffer = m_Device.createBuffer(bufferDesc);
	m_Queue.writeBuffer(indexBuffer, 0, indexData.data(), bufferDesc.size);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	wgpu::Buffer indexBuffer = m_Device.CreateBuffer(&bufferDesc);
	m_Queue.WriteBuffer(indexBuffer, 0, indexData.data(), bufferDesc.size);
#endif

	return indexBuffer;
}

void WebGPUDevice::WriteUniformBuffer(wgpu::Buffer buffer, size_t offset, void const* data, size_t size)
{
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	m_Queue.writeBuffer(buffer, offset, data, size);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	m_Queue.WriteBuffer(buffer, offset, data, size);
#endif
}


void WebGPUDevice::CreateTexture()
{
}

wgpu::Texture WebGPUDevice::CreateDepthTexture()
{
	wgpu::TextureFormat depthTextureFormat = wgpu::TextureFormat::Depth24Plus;
	// Create the depth texture
	wgpu::TextureDescriptor depthTextureDesc;
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	depthTextureDesc.dimension = wgpu::TextureDimension::_2D;
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	depthTextureDesc.dimension = wgpu::TextureDimension::e2D;
#endif
	depthTextureDesc.format = depthTextureFormat;
	depthTextureDesc.mipLevelCount = 1;
	depthTextureDesc.sampleCount = 1;
	depthTextureDesc.size = { 640, 480, 1 };
	depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
	depthTextureDesc.viewFormatCount = 1;

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	depthTextureDesc.viewFormats = (WGPUTextureFormat*)&depthTextureFormat;
	wgpu::Texture depthTexture = m_Device.createTexture(depthTextureDesc);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	depthTextureDesc.viewFormats = (wgpu::TextureFormat*)&depthTextureFormat;
	wgpu::Texture depthTexture = m_Device.CreateTexture(&depthTextureDesc);
#endif
	// std::cout << "Depth texture: " << depthTexture << std::endl;	// EMSCRIPTEN环境下无法打印：invalid operands to binary expression 

	return depthTexture;
}

wgpu::TextureView WebGPUDevice::CreateDepthTextureView(wgpu::Texture depthTexture)
{
	wgpu::TextureFormat depthTextureFormat = wgpu::TextureFormat::Depth24Plus;

	// Create the view of the depth texture manipulated by the rasterizer
	wgpu::TextureViewDescriptor depthTextureViewDesc;
	depthTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
	depthTextureViewDesc.baseArrayLayer = 0;
	depthTextureViewDesc.arrayLayerCount = 1;
	depthTextureViewDesc.baseMipLevel = 0;
	depthTextureViewDesc.mipLevelCount = 1;
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	depthTextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	depthTextureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
#endif
	depthTextureViewDesc.format = depthTextureFormat;
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	wgpu::TextureView depthTextureView = depthTexture.createView(depthTextureViewDesc);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	wgpu::TextureView depthTextureView = depthTexture.CreateView(&depthTextureViewDesc);
#endif
	// std::cout << "Depth texture view: " << depthTextureView << std::endl;	// EMSCRIPTEN环境下无法打印：invalid operands to binary expression

	return depthTextureView;
}

void WebGPUDevice::CreateSampler()
{
}


// Bind Group Layout
void WebGPUDevice::CreateUniformBindGroupLayoutEntries(std::vector<UniformLayoutEntryInfo>& uniformInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries)
{
	auto uniformCount = static_cast<uint32_t>(uniformInfos.size());
	for(uint32_t i = 0; i < uniformCount; i++)
	{
	#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
		wgpu::BindGroupLayoutEntry bindingLayout = wgpu::Default;
	#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
		wgpu::BindGroupLayoutEntry bindingLayout{};
	#endif
		bindingLayout.binding = uniformInfos[i].binding;
		bindingLayout.visibility = uniformInfos[i].visibility;
		bindingLayout.buffer.type = wgpu::BufferBindingType::Uniform;
		bindingLayout.buffer.minBindingSize = uniformInfos[i].minBindingSize;

		bindGroupLayoutEntries.push_back(bindingLayout);
	}
}

void WebGPUDevice::CreateTextureBindGroupLayoutEntries(std::vector<TextureLayoutEntryInfo>& textureInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries)
{
	auto textureCount = static_cast<uint32_t>(textureInfos.size());
	for(uint32_t i = 0; i < textureCount; i++)
	{
	#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
		wgpu::BindGroupLayoutEntry bindingLayout = wgpu::Default;
	#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
		wgpu::BindGroupLayoutEntry bindingLayout{};
	#endif
		bindingLayout.binding = textureInfos[i].binding;
		bindingLayout.visibility = textureInfos[i].visibility;
		bindingLayout.texture.sampleType = textureInfos[i].sampleType;
		bindingLayout.texture.viewDimension = textureInfos[i].viewDimension;
		// bindingLayout.texture.multisampled = textureInfos[i].multisampled;

		bindGroupLayoutEntries.push_back(bindingLayout);
	}
}

void WebGPUDevice::CreateStorageTextureBindGroupLayoutEntries(std::vector<StorageTextureLayoutEntryInfo>& storageTextureInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries)
{
	auto storageTextureCount = static_cast<uint32_t>(storageTextureInfos.size());
	for(uint32_t i = 0; i < storageTextureCount; i++)
	{
	#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
		wgpu::BindGroupLayoutEntry bindingLayout = wgpu::Default;
	#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
		wgpu::BindGroupLayoutEntry bindingLayout{};
	#endif
		bindingLayout.binding = storageTextureInfos[i].binding;
		bindingLayout.visibility = storageTextureInfos[i].visibility;
		bindingLayout.storageTexture.access = storageTextureInfos[i].access;
		bindingLayout.storageTexture.format = storageTextureInfos[i].format;
		bindingLayout.storageTexture.viewDimension = storageTextureInfos[i].viewDimension;

		bindGroupLayoutEntries.push_back(bindingLayout);
	}
}

void WebGPUDevice::CreateSamplerBindGroupLayoutEntries(std::vector<SamplerLayoutEntryInfo>& samplerInfos, std::vector<wgpu::BindGroupLayoutEntry>& bindGroupLayoutEntries)
{
	auto samplerCount = static_cast<uint32_t>(samplerInfos.size());
	for(uint32_t i = 0; i < samplerCount; i++)
	{
		wgpu::BindGroupLayoutEntry bindingLayout;
		bindingLayout.binding = samplerInfos[i].binding;
		bindingLayout.visibility = samplerInfos[i].visibility;
		bindingLayout.sampler.type = samplerInfos[i].type;

		bindGroupLayoutEntries.push_back(bindingLayout);
	}
}

wgpu::BindGroupLayout WebGPUDevice::CreateBindGroupLayout(std::vector<wgpu::BindGroupLayoutEntry>& entries)
{
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
	bindGroupLayoutDesc.entryCount = static_cast<uint32_t>(entries.size());
	bindGroupLayoutDesc.entries = entries.data();
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	return m_Device.createBindGroupLayout(bindGroupLayoutDesc);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	return m_Device.CreateBindGroupLayout(&bindGroupLayoutDesc);
#endif
}

// Bind Group
void WebGPUDevice::CreateUniformBindGroupEntry(std::vector<UniformEntryInfo>& uniformInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries)
{
	auto uniformCount = static_cast<uint32_t>(uniformInfos.size());
	for(uint32_t i = 0; i < uniformCount; i++)
	{
		wgpu::BindGroupEntry entry;
		entry.binding = uniformInfos[i].binding;
		entry.buffer = uniformInfos[i].buffer;
		entry.offset = uniformInfos[i].offset;
		entry.size = uniformInfos[i].size;

		bindGroupEntries.push_back(entry);
	}
}

void WebGPUDevice::CreateTextureBindGroupEntry(std::vector<TextureEntryInfo>& textureInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries)
{
	auto textureCount = static_cast<uint32_t>(textureInfos.size());
	for(uint32_t i = 0; i < textureCount; i++)
	{
		wgpu::BindGroupEntry entry;
		entry.binding = textureInfos[i].binding;
		entry.textureView = textureInfos[i].textureView;

		bindGroupEntries.push_back(entry);
	}
}

void WebGPUDevice::CreateStorageTextureBindGroupEntry(std::vector<StorageTextureEntryInfo>& storageTextureInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries)
{
	auto storageTextureCount = static_cast<uint32_t>(storageTextureInfos.size());
	for(uint32_t i = 0; i < storageTextureCount; i++)
	{
		wgpu::BindGroupEntry entry;
		entry.binding = storageTextureInfos[i].binding;
		// entry.storageTextureView = storageTextureInfos[i].storageTextureView;

		bindGroupEntries.push_back(entry);
	}
}

void WebGPUDevice::CreateSamplerBindGroupEntry(std::vector<SamplerEntryInfo>& samplerInfos, std::vector<wgpu::BindGroupEntry>& bindGroupEntries)
{
	auto samplerCount = static_cast<uint32_t>(samplerInfos.size());
	for(uint32_t i = 0; i < samplerCount; i++)
	{
		wgpu::BindGroupEntry entry;
		entry.binding = samplerInfos[i].binding;
		entry.sampler = samplerInfos[i].sampler;

		bindGroupEntries.push_back(entry);
	}
}

wgpu::BindGroup WebGPUDevice::CreateBindGroup(std::vector<wgpu::BindGroupEntry>& entries, wgpu::BindGroupLayout bindGroupLayout, const std::string& label)
{
	wgpu::BindGroupDescriptor bindGroupDesc;
	bindGroupDesc.label = label.c_str();
	bindGroupDesc.entryCount = static_cast<uint32_t>(entries.size());
	bindGroupDesc.entries = entries.data();
	bindGroupDesc.layout = bindGroupLayout;
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	return m_Device.createBindGroup(bindGroupDesc);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	return m_Device.CreateBindGroup(&bindGroupDesc);
#endif
}


// State
wgpu::VertexState WebGPUDevice::CreateVertState(wgpu::ShaderModule& shaderModule) const
{
	wgpu::VertexState vertState;
	vertState.module = shaderModule;
	vertState.entryPoint = "fs_main";
	vertState.constantCount = 0;
	vertState.constants = nullptr;

	return vertState;
}

wgpu::BlendState WebGPUDevice::CreateBlendState() const
{
	wgpu::BlendState blendState;
	blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
	blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
	blendState.color.operation = wgpu::BlendOperation::Add;
	blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
	blendState.alpha.dstFactor = wgpu::BlendFactor::One;
	blendState.alpha.operation = wgpu::BlendOperation::Add;

	return blendState;
}

void WebGPUDevice::CreateColorState(wgpu::ColorTargetState& colorTarget, wgpu::BlendState& blendState) const
{
#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE)
	colorTarget.format = m_SwapChainFormat;	 // webgpu-native可以跑通. emscripten不行？？？!!!!!!!!!!!!!!!!!!!!!!!!!! ,似乎是因为这里在函数内赋值的原因
	colorTarget.blend = &blendState;
	colorTarget.writeMask = wgpu::ColorWriteMask::All;
#endif
}

wgpu::DepthStencilState WebGPUDevice::CreateDepthStencilState() const
{
	wgpu::TextureFormat depthTextureFormat = wgpu::TextureFormat::Depth24Plus;

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	wgpu::DepthStencilState depthStencilState = wgpu::Default;
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	wgpu::DepthStencilState depthStencilState{};
#endif
	
	depthStencilState.depthCompare = wgpu::CompareFunction::Less;
	depthStencilState.depthWriteEnabled = true;
	depthStencilState.format = depthTextureFormat;
	depthStencilState.stencilReadMask = 0;
	depthStencilState.stencilWriteMask = 0;

	return depthStencilState;
}

void WebGPUDevice::CreateFragState(wgpu::FragmentState& fragState, wgpu::ShaderModule& shaderModule, wgpu::ColorTargetState& colorTarget) const
{
	fragState.module = shaderModule;
	fragState.entryPoint = "fs_main";
	fragState.constantCount = 0;
	fragState.constants = nullptr;
	fragState.targetCount = 1;
	fragState.targets = &colorTarget;
}


// Render Pipeline
wgpu::PipelineLayout WebGPUDevice::CreateRenderPipelineLayout(std::vector<wgpu::BindGroupLayout> bindGroupLayouts)
{
    wgpu::PipelineLayoutDescriptor layoutDesc{};
	layoutDesc.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)bindGroupLayouts.data();
	wgpu::PipelineLayout pipelineLayout = m_Device.createPipelineLayout(layoutDesc);
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	layoutDesc.bindGroupLayouts = (wgpu::BindGroupLayout*)bindGroupLayouts.data();
	wgpu::PipelineLayout pipelineLayout = m_Device.CreatePipelineLayout(&layoutDesc);
#endif
	return pipelineLayout;
}

wgpu::RenderPipeline WebGPUDevice::CreateRenderPipeline(
	wgpu::VertexBufferLayout& vertexBufferLayout,
	wgpu::BlendState& blendState,
	wgpu::PipelineLayout& pipelineLayout,
	wgpu::ShaderModule shaderModule)
{
    std::cout << "Creating render pipeline..." << std::endl;

	wgpu::RenderPipelineDescriptor pipelineDesc;
	wgpu::RenderPipeline pipeline = nullptr;

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	// ------------------------------------------------------------------------------

	// // Vertex fetch
	// std::vector<wgpu::VertexAttribute> vertexAttribs(2);

	// // Position attribute
	// vertexAttribs[0].shaderLocation = 0;
	// vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
	// //                                              ^ This was a 2
	// vertexAttribs[0].offset = 0;

	// // Color attribute
	// vertexAttribs[1].shaderLocation = 1;
	// vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
	// vertexAttribs[1].offset = 3 * sizeof(float);
	// //                        ^ This was a 2

	// wgpu::VertexBufferLayout vertexBufferLayout;
	// vertexBufferLayout.attributeCount = (uint32_t)vertexAttribs.size();
	// vertexBufferLayout.attributes = vertexAttribs.data();
	// vertexBufferLayout.arrayStride = 6 * sizeof(float);
	// //                               ^ This was a 5
	// vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

	// vertex shader
	pipelineDesc.vertex.bufferCount = 1;
	pipelineDesc.vertex.buffers = &vertexBufferLayout;
	pipelineDesc.vertex.module = shaderModule;
	pipelineDesc.vertex.entryPoint = "vs_main";
	pipelineDesc.vertex.constantCount = 0;
	pipelineDesc.vertex.constants = nullptr;

	//pipelineDesc.vertex = CreateVertState(shaderModule);

	// // blend mode
	// wgpu::BlendState blendState;
	// blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
	// blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
	// blendState.color.operation = wgpu::BlendOperation::Add;
	// blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
	// blendState.alpha.dstFactor = wgpu::BlendFactor::One;
	// blendState.alpha.operation = wgpu::BlendOperation::Add;

	// Color, Depth
	wgpu::ColorTargetState colorTargetState;
	colorTargetState.format = m_SwapChainFormat;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = wgpu::ColorWriteMask::All;

	// wgpu::DepthStencilState depthStencilState{};
	// depthStencilState.format = wgpu::TextureFormat::Depth32Float;
	// depthStencilState.depthCompare = wgpu::CompareFunction::Always;

	// fragment shader
	wgpu::FragmentState fragmentState;
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";
	fragmentState.constantCount = 0;
	fragmentState.constants = nullptr;
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTargetState;

	//wgpu::FragmentState fragmentState;
	//wgpu::ColorTargetState colorState;
	//CreateColorState(colorState, blendState);
	//CreateFragState(fragmentState, shaderModule, colorState);
	//pipelineDesc.fragment = &fragmentState;

	// pipelineDesc.depthStencil = &depthStencilState;
	pipelineDesc.depthStencil = nullptr;
	pipelineDesc.fragment = &fragmentState;
	// ------------------------------------------------------------------------------

    // sample setting
	pipelineDesc.multisample.count = 1;
	pipelineDesc.multisample.mask = ~0u;
	pipelineDesc.multisample.alphaToCoverageEnabled = false;

	// ------------------------------------------------------------------------------

	// primitive setting
	pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
	pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
	pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
	pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

	// ------------------------------------------------------------------------------

    // pipeline layout descriptor
    // wgpu::PipelineLayoutDescriptor layoutDesc{};
	// layoutDesc.bindGroupLayoutCount = 1;
	// layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayout;
	// wgpu::PipelineLayout pipelineLayout = m_Device.createPipelineLayout(layoutDesc);
	pipelineDesc.layout = pipelineLayout;

	// ------------------------------------------------------------------------------
    
	// create render pipeline
    pipeline = m_Device.createRenderPipeline(pipelineDesc);

#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)

	//
	pipelineDesc.vertex.bufferCount = 1;
	pipelineDesc.vertex.buffers = &vertexBufferLayout;
	pipelineDesc.vertex.module = shaderModule;
	pipelineDesc.vertex.entryPoint = "vs_main";
	pipelineDesc.vertex.constantCount = 0;
	pipelineDesc.vertex.constants = nullptr;

	// Color, Depth
    wgpu::ColorTargetState colorTargetState{};
	colorTargetState.blend = &blendState;
    colorTargetState.format = m_SwapChainFormat;
	colorTargetState.writeMask = wgpu::ColorWriteMask::All;

	wgpu::DepthStencilState depthStencilState{};
	depthStencilState.format = wgpu::TextureFormat::Depth32Float;
	depthStencilState.depthCompare = wgpu::CompareFunction::Always;

	//
	wgpu::FragmentState fragmentState{};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";
	fragmentState.constantCount = 0,
	fragmentState.constants = nullptr,
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTargetState;

	//
	pipelineDesc.depthStencil = &depthStencilState;
	pipelineDesc.fragment = &fragmentState;

    // sample setting
	pipelineDesc.multisample.count = 1;
	pipelineDesc.multisample.mask = ~0u;
	pipelineDesc.multisample.alphaToCoverageEnabled = false;

	// primitive setting
	pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
	pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
	pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
	pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

	//
	pipelineDesc.layout = pipelineLayout;

	//
    pipeline = m_Device.CreateRenderPipeline(&pipelineDesc);
#endif //
	// std::cout << "Render pipeline: " << pipeline << std::endl;	// EMSCRIPTEN环境下无法打印：invalid operands to binary expression
	return pipeline;
}



// Draw
// void WebGPUDevice::Draw(std::shared_ptr<Mesh> mesh, glm::vec4& clearColor)
// {
// 	wgpu::TextureView nextTexture = m_SwapChain.getCurrentTextureView();
// 	if (!nextTexture) {
// 		std::cerr << "Cannot acquire next swap chain texture" << std::endl;
// 		return;
// 	}

//     // 创建指令编码器
//     wgpu::CommandEncoderDescriptor commandEncoderDesc;
//     commandEncoderDesc.label = "Command Encoder";
//     wgpu::CommandEncoder encoder = m_Device.createCommandEncoder(commandEncoderDesc);
    
//     // 开始通道编码	
//     wgpu::RenderPassColorAttachment renderPassColorAttachment{};
//     renderPassColorAttachment.view = nextTexture;
//     renderPassColorAttachment.resolveTarget = nullptr;
//     renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
//     renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
//     renderPassColorAttachment.clearValue = wgpu::Color{ clearColor.r, clearColor.g, clearColor.b, clearColor.a };

//     wgpu::RenderPassDescriptor renderPassDesc{};
//     renderPassDesc.colorAttachmentCount = 1;
//     renderPassDesc.colorAttachments = &renderPassColorAttachment;
//     renderPassDesc.depthStencilAttachment = nullptr;
//     renderPassDesc.timestampWriteCount = 0;
//     renderPassDesc.timestampWrites = nullptr;
//     wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);

//     // 设置渲染通道的属性
//     renderPass.setPipeline(mesh->GetPipeline());
//     renderPass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBufferSize());
//     renderPass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetIndexBufferSize());
//     renderPass.setBindGroup(0, mesh->GetBindGroup(), 0, nullptr);
//     renderPass.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0); //

//     // 结束渲染通道通道编码
//     renderPass.end();
//     renderPass.release();

// 	//
// 	nextTexture.release();

//     // 结束指令编码
//     wgpu::CommandBufferDescriptor cmdBufferDescriptor{};
//     cmdBufferDescriptor.label = "Command buffer";
//     wgpu::CommandBuffer command = encoder.finish(cmdBufferDescriptor);
//     encoder.release();

//     // 提交指令缓冲
//     m_Queue.submit(command);
//     command.release();

// 	//
//     m_SwapChain.present();

// #ifdef WEBGPU_BACKEND_DAWN
//     // Check for pending error callbacks
//     m_Device.tick();
// #endif
// }

std::optional<Frame> WebGPUDevice::Begin(glm::vec4& clearColor)
{
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)

	wgpu::TextureView nextTexture = m_SwapChain.getCurrentTextureView();
	if (!nextTexture) {
		std::cerr << "Cannot acquire next swap chain texture" << std::endl;
		return std::nullopt;
	}

	// 创建指令编码器
    wgpu::CommandEncoderDescriptor commandEncoderDesc;
    commandEncoderDesc.label = "Command Encoder";
    wgpu::CommandEncoder encoder = m_Device.createCommandEncoder(commandEncoderDesc);
	std::cout << "Create Command Encoder: " << encoder << std::endl;

    // 开始通道编码	
    wgpu::RenderPassColorAttachment renderPassColorAttachment{};
    renderPassColorAttachment.view = nextTexture;
    renderPassColorAttachment.resolveTarget = nullptr;
    renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
    renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
    renderPassColorAttachment.clearValue = wgpu::Color{ clearColor.r, clearColor.g, clearColor.b, clearColor.a };

    wgpu::RenderPassDescriptor renderPassDesc{};
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &renderPassColorAttachment;
    renderPassDesc.depthStencilAttachment = nullptr;
    // renderPassDesc.timestampWriteCount = 0; // webgpu-native可以跑通？？？!!!!!!!!!!!!!!!!!!!!!!!!!! 
    renderPassDesc.timestampWrites = nullptr;
    wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);

#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
    wgpu::TextureView nextTexture = m_SwapChain.GetCurrentTextureView();

	// 创建指令编码器
    wgpu::CommandEncoderDescriptor commandEncoderDesc;
    commandEncoderDesc.label = "Command Encoder";
	wgpu::CommandEncoder encoder = m_Device.CreateCommandEncoder(&commandEncoderDesc);

    // 开始通道编码	
    wgpu::RenderPassColorAttachment renderPassColorAttachment{};
	renderPassColorAttachment.view = nextTexture;
	renderPassColorAttachment.resolveTarget = nullptr;
	renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
	renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
	renderPassColorAttachment.clearValue = wgpu::Color{ clearColor.r, clearColor.g, clearColor.b, clearColor.a };

	wgpu::TextureDescriptor textureDesc{};
	textureDesc.usage = wgpu::TextureUsage::RenderAttachment;
	textureDesc.size = { m_Width, m_Height, 1 };
	textureDesc.format = wgpu::TextureFormat::Depth32Float;
	wgpu::TextureView depthStencilView = m_Device.CreateTexture(&textureDesc).CreateView();

	wgpu::RenderPassDepthStencilAttachment depthStencilAttachment{};
	depthStencilAttachment.view = depthStencilView;
	depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
	depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
	depthStencilAttachment.depthClearValue = 0.f;

    wgpu::RenderPassDescriptor renderpassDesc{};
	renderpassDesc.colorAttachmentCount = 1;
	renderpassDesc.colorAttachments = &renderPassColorAttachment;
	renderpassDesc.depthStencilAttachment = &depthStencilAttachment;
	renderpassDesc.occlusionQuerySet = nullptr;
	renderpassDesc.timestampWrites = nullptr;
	// renderpassDesc.timestampWriteCount = 0;
    wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderpassDesc);
	
#endif

	return Frame{
		encoder,
		renderPass,
		nextTexture
	};
}

void WebGPUDevice::Draw(Frame& frame, std::shared_ptr<Mesh> mesh)
{
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN) 
    // 设置渲染通道的属性
    frame.renderPass.setPipeline(mesh->GetPipeline());
 
	// frame.renderPass.setBindGroup(0, mesh->GetBindGroup(), 0, nullptr);
	std::map<std::string, BindGroupData>& bindGroupsMap = mesh->GetBindGroups();
	for (auto& bindGroupData : bindGroupsMap)
	{
		frame.renderPass.setBindGroup(bindGroupData.second.groupIndex, bindGroupData.second.bindGroup, 0, nullptr);
	}

	frame.renderPass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBufferSize());
    frame.renderPass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetIndexBufferSize());

    frame.renderPass.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);	//
	// frame.renderPass.draw(mesh->GetIndexCount(), 1, 0, 0);	//
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)

    frame.renderPass.SetPipeline(mesh->GetPipeline());
	std::map<std::string, BindGroupData>& bindGroupsMap = mesh->GetBindGroups();
	for (auto& bindGroupData : bindGroupsMap)
	{
		frame.renderPass.SetBindGroup(bindGroupData.second.groupIndex, bindGroupData.second.bindGroup, 0, nullptr);
	}

	frame.renderPass.SetVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBufferSize());
    frame.renderPass.SetIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetIndexBufferSize());

    frame.renderPass.Draw(mesh->GetIndexCount());
#endif
}

void WebGPUDevice::End(Frame& frame)
{
#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
	// 结束渲染通道通道编码
    frame.renderPass.end();
    frame.renderPass.release();

	//
	frame.textureView.release();

    // 结束指令编码
    wgpu::CommandBufferDescriptor cmdBufferDescriptor{};
    cmdBufferDescriptor.label = "Command buffer";
    wgpu::CommandBuffer command = frame.encoder.finish(cmdBufferDescriptor);
    frame.encoder.release();

    // 提交指令缓冲
    m_Queue.submit(command);
    command.release();
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
	frame.renderPass.End();
    // frame.textureView.Destroy(); ??? 暂时不知道如何释放资源

    wgpu::CommandBuffer command = frame.encoder.Finish();
    m_Queue.Submit(1, &command);
#endif

// 浏览器会自动处理SwapChain，而桌面端的Window，则需要手动处理
#ifndef __EMSCRIPTEN__
	//

	#if defined(IE_DAWN_NATIVE) // WEBGPU_BACKEND_DAWN
    	m_SwapChain.Present();
		// Check for pending error callbacks
		// m_Device.tick();
	#elif defined(IE_WGPU_NATIVE)
	    m_SwapChain.present();
	#endif
#endif
}

