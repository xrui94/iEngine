#include "Window.h"

#ifdef IE_GLFW_WINDOW
	#include "GlfwWindow.h"
#elif defined(IE_HTML_WINDOW)
	#include "HtmlWindow.h"
#endif

#include <memory>
#include <iostream>

std::unique_ptr<Window> Window::Create(const WindowProps& props)
{
#ifdef IE_GLFW_WINDOW
    return std::make_unique<GlfwWindow>(props);
#elif defined(IE_QT_WINDOW)
    std::cerr << "Unsupported platform!" << std::endl;
    return nullptr;
#elif defined(IE_HTML_WINDOW)
    return std::make_unique<HtmlWindow>(props);
    return nullptr;
#else
    std::cerr << "Unknown platform!" << std::endl;
    return nullptr;
#endif
}
