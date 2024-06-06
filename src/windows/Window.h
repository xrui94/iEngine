#pragma once

#if defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

#include <string>
#include <memory>

struct WindowProps
{
#if defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    std::string ContainerId;
    std::string CanvasId;
    std::string Style;
#else
    std::string Title;
#endif
    uint32_t Width;
    uint32_t Height;

#if defined(IE_WGPU_NATIVE)
    WindowProps(const std::string& title = "iEngine",
                uint32_t width = 960,
                uint32_t height = 640)
        : Title(title), Width(width), Height(height)
    {
    }
#elif defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    WindowProps(std::string containerId,
                const std::string& canvasId = "xr-engine",
                uint32_t width = 960,
                uint32_t height = 640,
                const std::string& style = "")
        : ContainerId(containerId), CanvasId(canvasId), Width(width), Height(height), Style(style)
    {
    }
#endif
};

// Interface representing a desktop system based Window
class Window
{
public:
    // using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() = default;

public:
    //
    static std::unique_ptr<Window> Create(const WindowProps& props);

    virtual Window& Get() { return *this; }

    virtual void* GetNativeWindow() const = 0;

    virtual uint32_t GetWidth() const = 0;

    virtual uint32_t GetHeight() const = 0;

    //
#if defined(IE_WGPU_NATIVE)

    virtual void OnResize(uint32_t width, uint32_t height) = 0;

    virtual void OnMouseButton(int32_t button, int32_t action, int32_t/* mods*/) = 0;

    virtual void OnMouseMove(double xPos, double yPos) = 0;

    virtual void OnScroll(double xOffset, double yOffset) = 0;

#elif defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)

    virtual EM_BOOL OnMouseClick(int eventType, const EmscriptenMouseEvent *e, void *userData) = 0;

    virtual EM_BOOL OnMouseDbClick(int eventType, const EmscriptenMouseEvent *e, void *userData) = 0;

    virtual EM_BOOL OnMouseDown(int eventType, const EmscriptenMouseEvent *e, void *userData) = 0;

    virtual EM_BOOL OnMouseUp(int eventType, const EmscriptenMouseEvent *e, void *userData) = 0;

    virtual EM_BOOL OnMouseMove(int eventType, const EmscriptenMouseEvent *e, void *userData) = 0;

    virtual EM_BOOL OnMouseWheel(int eventType, const EmscriptenWheelEvent *e, void *userData) = 0;

    virtual EM_BOOL OnResize(int eventType, const EmscriptenUiEvent *e, void *userData) = 0;

#endif

    //
    virtual bool GetSwapchainState() = 0;

    virtual void SetSwapchainState(bool updated) = 0;
};