#include "Constants.h"
#include "PerspectiveCamera.h"
#include "renderers/webgpu/WebGPUDevice.h"

#include <vector>


PerspectiveCamera::PerspectiveCamera(float aspect)
	: Camera(),
	m_Position({0.f, 0.f, 0.f}), m_Up({ 0.f, 1.f, 0.f }),
	m_Fov(45), m_Near(0.01f), m_Far(1000.f), m_Aspect(aspect),
	m_CameraUniformBuffer(nullptr)
{
}

PerspectiveCamera::~PerspectiveCamera()
{
	Destroy();
}

void PerspectiveCamera::Destroy()
{
	// m_CameraUniformBuffer.destroy();
	// m_CameraUniformBuffer.release();

	// m_BindGroup.release();
	// m_BindGroupLayout.release();

	#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
		m_CameraUniformBuffer.destroy();
		m_CameraUniformBuffer.release();

		m_BindGroup.release();
		m_BindGroupLayout.release();
	#elif defined(IE_ONLY_EMSCRIPTEN)
		m_CameraUniformBuffer.Destroy();
		// m_CameraUniformBuffer.Release();

		// m_BindGroup.Release();
		// m_BindGroupLayout.Release();
	#endif
}

void PerspectiveCamera::Init()
{
	// Upload the initial value of the uniforms
	m_CameraUniformBuffer = WebGPUDevice::Get().CreateUniformBuffer<GlobalUniforms>("GlobalUniforms");
	m_GlobalUniforms.viewMatrix = glm::lookAt(glm::vec3(-2.0f, -3.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0, 0, 1));
	m_GlobalUniforms.projectionMatrix = glm::perspective(45 * PI / 180, m_Aspect, 0.01f, 100.0f);
	m_GlobalUniforms.time = 1.0f;
	m_GlobalUniforms.gamma = WebGPUDevice::Get().GetTextureFormatGamma();
	WebGPUDevice::Get().WriteUniformBuffer(m_CameraUniformBuffer, 0, &m_GlobalUniforms, sizeof(GlobalUniforms));

	// BindGroupLayout
	std::vector<UniformLayoutEntryInfo> uniformLayoutEntryInfos = {
		UniformLayoutEntryInfo {
			0,
			wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
			sizeof(GlobalUniforms)
		}
	};
	std::vector<wgpu::BindGroupLayoutEntry> uniformBindGroupLayoutEntries;
	WebGPUDevice::Get().CreateUniformBindGroupLayoutEntries(uniformLayoutEntryInfos, uniformBindGroupLayoutEntries);
	m_BindGroupLayout = WebGPUDevice::Get().CreateBindGroupLayout(uniformBindGroupLayoutEntries);

	// BindGroup
	std::vector<UniformEntryInfo> uniformEntryInfos = {
		{
			0,
			m_CameraUniformBuffer,
			0,
			sizeof(GlobalUniforms)
		}
	};
	std::vector<wgpu::BindGroupEntry> uniformBindGroupEntries;
	WebGPUDevice::Get().CreateUniformBindGroupEntry(uniformEntryInfos, uniformBindGroupEntries);
	m_BindGroup = WebGPUDevice::Get().CreateBindGroup(uniformBindGroupEntries, m_BindGroupLayout);

	//
	m_IsInitialized = true;
}

void PerspectiveCamera::UpdateViewMatrix()
{
	float cx = cos(m_XAngle);
	float sx = sin(m_XAngle);
	float cy = cos(m_YAngle);
	float sy = sin(m_YAngle);
	glm::vec3 position = glm::vec3(cx * cy, sx * cy, sy) * std::exp(-GetZoom());

	//
	m_GlobalUniforms.viewMatrix = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0, 0, 1));
	//m_GlobalUniforms.viewMatrix = glm::mat4(1.0f);
	WebGPUDevice::Get().WriteUniformBuffer(
		m_CameraUniformBuffer,
		offsetof(GlobalUniforms, viewMatrix),
		&m_GlobalUniforms.viewMatrix,
		sizeof(GlobalUniforms::viewMatrix)
	);

	//
	m_GlobalUniforms.cameraWorldPosition = position;
	WebGPUDevice::Get().WriteUniformBuffer(
		m_CameraUniformBuffer,
		offsetof(GlobalUniforms, cameraWorldPosition),
		&m_GlobalUniforms.cameraWorldPosition,
		sizeof(GlobalUniforms::cameraWorldPosition)
	);
}

void PerspectiveCamera::UpdateProjectionMatrix(float aspect)
{
	std::cout << "aspect = " << aspect << std::endl;
	// Update projection matrix
	m_GlobalUniforms.projectionMatrix = glm::perspective(45 * PI / 180, 1.0f, 0.01f, 1000.0f);
	//m_GlobalUniforms.projectionMatrix = glm::mat4(1.0f);
	WebGPUDevice::Get().WriteUniformBuffer(
		m_CameraUniformBuffer,
		offsetof(GlobalUniforms, projectionMatrix),
		&m_GlobalUniforms.projectionMatrix,
		sizeof(GlobalUniforms::projectionMatrix)
	);
}

void PerspectiveCamera::UpdateDragInertia()
{
	// constexpr float eps = 1e-4f;
	// // Apply inertia only when the user released the click.
	// if (!m_drag.active) {
	// 	// Avoid updating the matrix when the velocity is no longer noticeable
	// 	if (std::abs(m_drag.velocity.x) < eps && std::abs(m_drag.velocity.y) < eps) {
	// 		return;
	// 	}
	// 	GetAngles() += m_drag.velocity;
	// 	GetAngles().y = glm::clamp(GetAngles().y, -PI / 2 + 1e-5f, PI / 2 - 1e-5f);
	// 	// Dampen the velocity so that it decreases exponentially and stops
	// 	// after a few frames.
	// 	m_drag.velocity *= m_drag.intertia;
	// 	updateViewMatrix();
	// }
}