#pragma once

#include "Window.h"
#include "../Scene.h"
#include "../cameras/Camera.h"

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/html5.h>

#include <mutex>
#include <condition_variable>

class HtmlWindow : public Window
{
public:
    HtmlWindow(const WindowProps& props);

public:
    virtual void* GetNativeWindow() const override { return static_cast<void*>(const_cast<HtmlWindow*>(this)); }

    uint32_t GetWidth() const override { return m_Width; }

    uint32_t GetHeight() const override { return m_Height; }

    std::string GetCanvasId() const { return m_CanvasId; }

    bool ShouleClose() const { return m_ShouleClose; }

    void SetScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene.get(); }

    void RegisterEvent();

    EM_BOOL OnMouseClick(int eventType, const EmscriptenMouseEvent *e, void *userData) override;

    EM_BOOL OnMouseDbClick(int eventType, const EmscriptenMouseEvent *e, void *userData) override;

    EM_BOOL OnMouseDown(int eventType, const EmscriptenMouseEvent *e, void *userData) override;

    EM_BOOL OnMouseUp(int eventType, const EmscriptenMouseEvent *e, void *userData) override;

    EM_BOOL OnMouseMove(int eventType, const EmscriptenMouseEvent *e, void *userData) override;

    EM_BOOL OnMouseWheel(int eventType, const EmscriptenWheelEvent *e, void *userData) override;

    EM_BOOL OnResize(int eventType, const EmscriptenUiEvent *e, void *userData) override;

    inline const char* emscripten_event_type_to_string(int eventType);

    const char* emscripten_result_to_string(EMSCRIPTEN_RESULT result);

    bool GetSwapchainState() override;

    void SetSwapchainState(bool updated) override;

private:
    void CreateCanvas(const std::string& containerId, const std::string& canvasId, const std::string& style);

private:
    emscripten::val m_Canvas;

    int32_t m_Width;

    int32_t m_Height;

    std::string m_CanvasId;

    bool m_ShouleClose;
    
    Scene* m_Scene;

    DragState m_DragState;

    bool m_SwapchainUpdated;    //

    std::mutex m_Mutex;

    std::condition_variable m_ConditionVar;
};
