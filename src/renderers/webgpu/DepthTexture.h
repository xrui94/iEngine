#pragma once

class DepthTexture
{
public:
    DepthTexture();
    ~DepthTexture();

private:
	wgpu::TextureFormat m_DepthTextureFormat;
	wgpu::Texture m_DepthTexture;
	wgpu::TextureView m_DepthTextureView;
};