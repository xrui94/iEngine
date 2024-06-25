#include "GlfwWindow.h"

#include "Scene.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <memory>


GlfwWindow::GlfwWindow(const WindowProps& props)
	: m_Scene(nullptr), m_Width(props.Width), m_Height(props.Height), m_SwapchainUpdated(true)
{
    //设置错误回调
	glfwSetErrorCallback([](int code, const char* errorDescription) {
		fprintf(stderr, "GLFW Error: %d,%s\n", code, errorDescription);
	});

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

	// OpenGL
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// WebGPU(Dawn Native, WGPU-Native)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    //创建窗口
    m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
		// 清理资源
    	glfwDestroyWindow(m_Window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

	// OpenGL
	//glfwMakeContextCurrent(m_Window);
}

void GlfwWindow::RegisterEvent()
{
    //
    glfwSetWindowUserPointer(m_Window, this);

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        auto that = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        if (that != nullptr) that->OnResize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    });

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
		auto that = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
		if (that != nullptr) that->OnMouseButton(button, action, mods);
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
		auto that = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
		if (that != nullptr) that->OnMouseMove(xpos, ypos);
	});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
		auto that = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
		if (that != nullptr) that->OnScroll(xoffset, yoffset);
	});
}

void GlfwWindow::OnResize(uint32_t width, uint32_t height)
{
    if (m_Scene == nullptr) return;

	// Notify rendering thread that swapchain is updated
	SetSwapchainState(false);

    m_Scene->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

	if (width == 0 || height == 0) return;

	m_Scene->GetCamera()->UpdateProjectionMatrix(width / float(height));

	m_Scene->SetState(false);

	// Notify rendering thread that swapchain is updated
	//SetSwapchainState(true);
}

void GlfwWindow::OnMouseButton(int32_t button, int32_t action, int32_t/* mods*/)
{
    if (m_Scene == nullptr) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
		switch (action) {
		case GLFW_PRESS:
			m_Drag.active = true;
			double xpos, ypos;
			glfwGetCursorPos(m_Window, &xpos, &ypos);
			m_Drag.startMouse = glm::vec2(-(float)xpos, (float)ypos);
			m_Drag.startCameraState = { 
				glm::vec2(m_Scene->GetCamera()->GetXAngle(), m_Scene->GetCamera()->GetYAngle()),
				m_Scene->GetCamera()->GetZoom()
			};
			break;
		case GLFW_RELEASE:
			m_Drag.active = false;
			break;
		}
	}
}

void GlfwWindow::OnMouseMove(double xpos, double ypos)
{
	std::cout << "Mouse Move..." << std::endl;
    if (m_Scene == nullptr) return;

    if (m_Drag.active)
	{
		glm::vec2 currentMouse = glm::vec2(-(float)xpos, (float)ypos);
		glm::vec2 delta = (currentMouse - m_Drag.startMouse) * m_Drag.sensitivity;

		// m_CameraState.angles = m_Drag.startCameraState.angles + delta;
		m_Scene->GetCamera()->SetXAngle(m_Drag.startCameraState.angles.x + delta.x);
		m_Scene->GetCamera()->SetYAngle(m_Drag.startCameraState.angles.y + delta.y);

		// Clamp to avoid going too far when orbitting up/down
		// m_CameraState.angles.y = glm::clamp(m_CameraState.angles.y, -PI / 2 + 1e-5f, PI / 2 - 1e-5f);
		m_Scene->GetCamera()->SetYAngle(glm::clamp(m_Scene->GetCamera()->GetYAngle(), -PI / 2 + 1e-5f, PI / 2 - 1e-5f));

		// UpdateViewMatrix();
		m_Scene->GetCamera()->UpdateViewMatrix();

		// Inertia
		m_Drag.velocity = delta - m_Drag.previousDelta;
		m_Drag.previousDelta = delta;

		// Notify rendering thread that swapchain is updated
		//SetSwapchainState(true);
	}
}

void GlfwWindow::OnScroll(double, double yoffset)
{
	std::cout << "Mouse Scroll..." << std::endl;
    if (m_Scene == nullptr) return;

	m_Scene->GetCamera()->SetZoom(m_Scene->GetCamera()->GetZoom() + m_Drag.scrollSensitivity * static_cast<float>(yoffset));
	m_Scene->GetCamera()->SetZoom(glm::clamp(m_Scene->GetCamera()->GetZoom(), -2.0f, 2.0f));
	m_Scene->GetCamera()->UpdateViewMatrix();

	// Notify rendering thread that swapchain is updated
	//SetSwapchainState(true);
}

bool GlfwWindow::GetSwapchainState()
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_ConditionVar.wait(lock, [this] { return m_SwapchainUpdated; });
	// m_SwapchainUpdated = false;
	// return m_SwapchainUpdated;
	return true;
}

void GlfwWindow::SetSwapchainState(bool updated)
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_SwapchainUpdated = updated;
	m_ConditionVar.notify_one();
}

void GlfwWindow::UpdateSwapChainState(bool state)
{
	m_SwapchainUpdated = state;
}