#pragma once

#include "renderers/Renderer.h"
#include "cameras/Camera.h"

#include <vector>
#include <memory>

class Mesh;

class Scene
{
public:
    Scene(uint32_t width, uint32_t height, std::shared_ptr<Camera> camera = nullptr);
    ~Scene();

public:
    const std::string& GetId() { return m_Id; }

    std::shared_ptr<Camera>& GetCamera() { return m_Camera; }

    std::unique_ptr<Renderer>& GetRenderer() { return m_Renderer; }

    void AddCamera(std::shared_ptr<Camera> camera);

    void DestroyCamera();

    void AddMesh(std::shared_ptr<Mesh> mesh);

    void Render();

    void RenderOnce();

    void RenderLoop();

    void Clear();

    bool GetState() const { return m_IsOutdated; }

    void SetState(bool value) { m_IsOutdated = value; }

    void Resize(uint32_t width, uint32_t height);

    void Rotate();

    void Translate();

    void Zoom();

    void Pickup();

private:
    std::string m_Id;

    bool m_IsOutdated;

    uint32_t m_Width;
    
    uint32_t m_Height;

    std::shared_ptr<Camera> m_Camera;

    std::unique_ptr<Renderer> m_Renderer;

    std::vector<std::shared_ptr<Mesh>> m_Meshes;
};