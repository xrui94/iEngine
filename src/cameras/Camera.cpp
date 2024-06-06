#include "Camera.h"

Camera::Camera()
    : m_IsInitialized(false), m_XAngle(0.8f), m_YAngle(0.5f), m_Zoom(-1.2f),
    m_BindGroupLayout(nullptr), m_BindGroup(nullptr)
{
}

Camera::~Camera()
{
}
