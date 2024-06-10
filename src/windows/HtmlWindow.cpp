#include "HtmlWindow.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <memory>

// static bool g_Active = false;

HtmlWindow::HtmlWindow(const WindowProps& props)
	: m_Scene(nullptr), m_Width(props.Width), m_Height(props.Height), m_CanvasId(props.CanvasId), m_ShouleClose(false), m_SwapchainUpdated(true)
{
    CreateCanvas(props.ContainerId, props.CanvasId, props.Style);
}

void HtmlWindow::CreateCanvas(const std::string& containerId, const std::string& canvasId, const std::string& style)
{
    m_Canvas = emscripten::val::global("document").call<emscripten::val>("createElement", emscripten::val("canvas"));
    // if (canvasId[0] == '#')
    // {
    //     m_Canvas.set("id", canvasId.substr(1).c_str());   // 当在JS中传过来的Canvas加了“#”号时，要去掉“#”号
    // }
    // else
    // {
    //     m_Canvas.set("id", canvasId.c_str());  // 在JS中传过来的Canvas没有“#”号时，则无须处理，直接使用
    // }
    // m_Canvas.set("id", canvasId.substr(1).c_str());   // 当在JS中传过来的Canvas加了“#”号时，要去掉“#”号
    m_Canvas.set("id", canvasId.c_str());  // 在JS中传过来的Canvas没有“#”号时，则无须处理，直接使用

    if (style.size() == 0)
    {
        m_Canvas.call<void>("setAttribute", emscripten::val("style"), emscripten::val("width: 100%; margin: 0; padding: 0"));
    }
    else
    {
        m_Canvas.call<void>("setAttribute", emscripten::val("style"), emscripten::val(style.c_str()));
    }

    //
    emscripten::val parentElement = emscripten::val::global("document").call<emscripten::val>("getElementById", emscripten::val(containerId.c_str()));
    parentElement.call<void>("appendChild", m_Canvas);

    // 将canvas实例添加到Module对象上
    EM_ASM_ARGS({
        let canvasId = UTF8ToString($0);
        if (canvasId.startsWith("#")) canvasId = canvasId.substr(1);
        const canvas = document.getElementById(canvasId);
        if (!canvas) throw "A valid HTML Canvas element is required.";
        canvas.addEventListener("webglcontextlost", (e) => { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
        Module["canvas"] = canvas;
    }, canvasId.c_str());
}

void HtmlWindow::RegisterEvent()
{
    // 加上“#”
    const char* canvasIdWithSelector = std::string("#").append(m_CanvasId).c_str();

    // 关于事件的定义：https://github.com/emscripten-core/emscripten/blob/main/system/include/emscripten/html5.h
    // EMSCRIPTEN_RESULT ret;

    EMSCRIPTEN_RESULT ret1 = emscripten_set_click_callback(canvasIdWithSelector, this, true, [](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL {
        auto that = static_cast<HtmlWindow*>(userData);
        return that->OnMouseClick(eventType, e/*, userData*/);
    });

    EMSCRIPTEN_RESULT ret2 = emscripten_set_dblclick_callback(canvasIdWithSelector, this, true, [](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL {
        auto that = static_cast<HtmlWindow*>(userData);
        return that->OnMouseDbClick(eventType, e/*, userData*/);
    });

    EMSCRIPTEN_RESULT ret3 = emscripten_set_mousedown_callback(canvasIdWithSelector, this, true, [](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL {
        auto that = static_cast<HtmlWindow*>(userData);
        return that->OnMouseDown(eventType, e/*, userData*/);
    });

    EMSCRIPTEN_RESULT ret4 = emscripten_set_mouseup_callback(canvasIdWithSelector, this, true, [](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL {
        auto that = static_cast<HtmlWindow*>(userData);
        return that->OnMouseUp(eventType, e/*, userData*/);
    });

    EMSCRIPTEN_RESULT ret5 = emscripten_set_mousemove_callback(canvasIdWithSelector, this, true, [](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL {
        auto that = static_cast<HtmlWindow*>(userData);
        return that->OnMouseMove(eventType, e/*, userData*/);
    });

    EMSCRIPTEN_RESULT ret6 = emscripten_set_wheel_callback(canvasIdWithSelector, this, true, [](int eventType, const EmscriptenWheelEvent *e, void *userData) -> EM_BOOL {
        auto that = static_cast<HtmlWindow*>(userData);
        return that->OnMouseWheel(eventType, e/*, userData*/);
    });

    EMSCRIPTEN_RESULT ret7 = emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, [](int eventType, const EmscriptenUiEvent *e, void *userData) -> EM_BOOL {
        auto that = static_cast<HtmlWindow*>(userData);
        return that->OnResize(eventType, e/*, userData*/);
    });
}

EM_BOOL HtmlWindow::OnMouseClick(int eventType, const EmscriptenMouseEvent *e/*, void *userData*/)
{
    if (eventType == EMSCRIPTEN_EVENT_CLICK && e->screenX == -500000)
    {
        printf("ERROR! Received an event to a callback that should have been unregistered!\n");
    }

    std::cout << "Processing mouse click event" << std::endl;
    std::cout << "screenX:" << e->screenX << std::endl;
    std::cout << "screenY:" << e->screenY << std::endl;
    std::cout << "clientX:" << e->clientX << std::endl;
    std::cout << "clientY:" << e->clientY << std::endl;
    std::cout << "targetX:" << e->targetX << std::endl;
    std::cout << "targetY:" << e->targetY << std::endl;

    return 0;
}

EM_BOOL HtmlWindow::OnMouseDbClick(int eventType, const EmscriptenMouseEvent *e/*, void *userData*/)
{
    std::cout << "Processing mouse double click event" << std::endl;
    std::cout << "screenX:" << e->screenX << std::endl;
    std::cout << "screenY:" << e->screenY << std::endl;
    std::cout << "clientX:" << e->clientX << std::endl;
    std::cout << "clientY:" << e->clientY << std::endl;
    std::cout << "targetX:" << e->targetX << std::endl;
    std::cout << "targetY:" << e->targetY << std::endl;
    
    return 0;
}

EM_BOOL HtmlWindow::OnMouseDown(int eventType, const EmscriptenMouseEvent *e/*, void *userData*/)
{
    m_DragState.active = true;
    // g_Active = true;
    std::cout << "Processing mouse down event" << std::endl;
    std::cout << "screenX:" << e->screenX << std::endl;
    std::cout << "screenY:" << e->screenY << std::endl;
    std::cout << "clientX:" << e->clientX << std::endl;
    std::cout << "clientY:" << e->clientY << std::endl;
    std::cout << "targetX:" << e->targetX << std::endl;
    std::cout << "targetY:" << e->targetY << std::endl;

    return 0;
}

EM_BOOL HtmlWindow::OnMouseUp(int eventType, const EmscriptenMouseEvent *e/*, void *userData*/)
{
    m_DragState.active = false;
    // g_Active = false;
    std::cout << "Processing mouse up event" << std::endl;
    std::cout << "screenX:" << e->screenX << std::endl;
    std::cout << "screenY:" << e->screenY << std::endl;
    std::cout << "clientX:" << e->clientX << std::endl;
    std::cout << "clientY:" << e->clientY << std::endl;
    std::cout << "targetX:" << e->targetX << std::endl;
    std::cout << "targetY:" << e->targetY << std::endl;
    
    return 0;
}

EM_BOOL HtmlWindow::OnMouseMove(int eventType, const EmscriptenMouseEvent *e/*, void *userData*/)
{
    if (m_Scene == nullptr) return false;

    if (m_DragState.active)  // not working
    // if (g_Active)
    {
        std::cout << "Processing mouse move event" << std::endl;
        std::cout << "screenX:" << e->screenX << std::endl;
        std::cout << "screenY:" << e->screenY << std::endl;
        std::cout << "clientX:" << e->clientX << std::endl;
        std::cout << "clientY:" << e->clientY << std::endl;
        std::cout << "targetX:" << e->targetX << std::endl;
        std::cout << "targetY:" << e->targetY << std::endl;

        glm::vec2 currentMouse = glm::vec2(-(float)e->clientX, (float)e->clientY);
		glm::vec2 delta = (currentMouse - m_DragState.startMouse) * m_DragState.sensitivity;

		// m_CameraState.angles = m_DragState.startCameraState.angles + delta;
		m_Scene->GetCamera()->SetXAngle(m_DragState.startCameraState.angles.x + delta.x);
		m_Scene->GetCamera()->SetYAngle(m_DragState.startCameraState.angles.y + delta.y);

		// Clamp to avoid going too far when orbitting up/down
		// m_CameraState.angles.y = glm::clamp(m_CameraState.angles.y, -PI / 2 + 1e-5f, PI / 2 - 1e-5f);
		m_Scene->GetCamera()->SetYAngle(glm::clamp(m_Scene->GetCamera()->GetYAngle(), -PI / 2 + 1e-5f, PI / 2 - 1e-5f));

		// UpdateViewMatrix();
		m_Scene->GetCamera()->UpdateViewMatrix();

		// Inertia
		m_DragState.velocity = delta - m_DragState.previousDelta;
		m_DragState.previousDelta = delta;

		// Notify rendering thread that swapchain is updated
		SetSwapchainState(true);
    }

    return true;
}

EM_BOOL HtmlWindow::OnMouseWheel(int eventType, const EmscriptenWheelEvent *e/*, void *userData*/)
{
    std::cout << "Processing mouse wheel event" << std::endl;
    std::cout << "deltaX:" << e->deltaX << std::endl;
    std::cout << "deltaY:" << e->deltaY << std::endl;

    if (m_Scene == nullptr) return false;

	m_Scene->GetCamera()->SetZoom(m_Scene->GetCamera()->GetZoom() + m_DragState.scrollSensitivity * static_cast<float>(e->deltaY));
	m_Scene->GetCamera()->SetZoom(glm::clamp(m_Scene->GetCamera()->GetZoom(), -2.0f, 2.0f));
	m_Scene->GetCamera()->UpdateViewMatrix();

	// Notify rendering thread that swapchain is updated
	SetSwapchainState(true);

    return true;
}

EM_BOOL HtmlWindow::OnResize(int eventType, const EmscriptenUiEvent *e/*, void *userData*/)
{
    int width = (int) e->windowInnerWidth;
    int height = (int) e->windowInnerHeight;

    std::cout << "Processing resize event" << std::endl;
    std::cout << "width:" << width << std::endl;
    std::cout << "height:" << height << std::endl;

    if (m_Scene == nullptr) return false;

    m_Scene->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

	m_Scene->GetCamera()->UpdateProjectionMatrix(width / float(height));

	// Notify rendering thread that swapchain is updated
	SetSwapchainState(true);

    return true;
}

inline const char* HtmlWindow::emscripten_event_type_to_string(int eventType)
{
    const char *events[] = {"(invalid)", "(none)", "keypress", "keydown", "keyup", "click", "mousedown", "mouseup", "dblclick", "mousemove", "wheel", "resize",
                            "scroll", "blur", "focus", "focusin", "focusout", "deviceorientation", "devicemotion", "orientationchange", "fullscreenchange", "pointerlockchange",
                            "visibilitychange", "touchstart", "touchend", "touchmove", "touchcancel", "gamepadconnected", "gamepaddisconnected", "beforeunload",
                            "batterychargingchange", "batterylevelchange", "webglcontextlost", "webglcontextrestored", "(invalid)"};
    ++eventType;
    if (eventType < 0)
        eventType = 0;
    if (eventType >= sizeof(events) / sizeof(events[0]))
        eventType = sizeof(events) / sizeof(events[0]) - 1;
    return events[eventType];
}

const char* HtmlWindow::emscripten_result_to_string(EMSCRIPTEN_RESULT result)
{
    if (result == EMSCRIPTEN_RESULT_SUCCESS)
        return "EMSCRIPTEN_RESULT_SUCCESS";
    if (result == EMSCRIPTEN_RESULT_DEFERRED)
        return "EMSCRIPTEN_RESULT_DEFERRED";
    if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
        return "EMSCRIPTEN_RESULT_NOT_SUPPORTED";
    if (result == EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED)
        return "EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED";
    if (result == EMSCRIPTEN_RESULT_INVALID_TARGET)
        return "EMSCRIPTEN_RESULT_INVALID_TARGET";
    if (result == EMSCRIPTEN_RESULT_UNKNOWN_TARGET)
        return "EMSCRIPTEN_RESULT_UNKNOWN_TARGET";
    if (result == EMSCRIPTEN_RESULT_INVALID_PARAM)
        return "EMSCRIPTEN_RESULT_INVALID_PARAM";
    if (result == EMSCRIPTEN_RESULT_FAILED)
        return "EMSCRIPTEN_RESULT_FAILED";
    if (result == EMSCRIPTEN_RESULT_NO_DATA)
        return "EMSCRIPTEN_RESULT_NO_DATA";
    return "Unknown EMSCRIPTEN_RESULT!";
}

bool HtmlWindow::GetSwapchainState()
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_ConditionVar.wait(lock, [this] { return m_SwapchainUpdated; });
	return m_SwapchainUpdated;
}

void HtmlWindow::SetSwapchainState(bool updated)
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_SwapchainUpdated = updated;
	m_ConditionVar.notify_one();
}
