#include "DepthTexture.h"

DepthTexture::DepthTexture(uint32_t width, uint32_t height)
    : m_DepthTextureFormat(wgpu::TextureFormat::Depth24Plus),
	m_DepthTexture(nullptr),
	wgpu::TextureView m_DepthTextureView(nullptr)
{
	// Create the depth texture
	TextureDescriptor depthTextureDesc;
	depthTextureDesc.dimension = TextureDimension::_2D;
	depthTextureDesc.format = m_DepthTextureFormat;
	depthTextureDesc.mipLevelCount = 1;
	depthTextureDesc.sampleCount = 1;
	depthTextureDesc.size = { width, height, 1 };
	depthTextureDesc.usage = TextureUsage::RenderAttachment;
	depthTextureDesc.viewFormatCount = 1;
	depthTextureDesc.viewFormats = (WGPUTextureFormat*)&m_DepthTextureFormat;
	m_DepthTexture = m_device.createTexture(depthTextureDesc);
	std::cout << "Depth texture: " << m_DepthTexture << std::endl;

	// Create the view of the depth texture manipulated by the rasterizer
	TextureViewDescriptor depthTextureViewDesc;
	depthTextureViewDesc.aspect = TextureAspect::DepthOnly;
	depthTextureViewDesc.baseArrayLayer = 0;
	depthTextureViewDesc.arrayLayerCount = 1;
	depthTextureViewDesc.baseMipLevel = 0;
	depthTextureViewDesc.mipLevelCount = 1;
	depthTextureViewDesc.dimension = TextureViewDimension::_2D;
	depthTextureViewDesc.format = m_DepthTextureFormat;
	m_DepthTextureView = m_DepthTexture.createView(depthTextureViewDesc);
	std::cout << "Depth texture view: " << m_DepthTextureView << std::endl;

	if (m_DepthTextureView == nullptr)
    {
        std::cerr << "Failed to create depth texture." << std::endl;
    }
}

DepthTexture::~DepthTexture()
{
    m_DepthTextureView.release();
	m_DepthTexture.destroy();
	m_DepthTexture.release();
}