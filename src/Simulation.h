#pragma once

#include "Application.h"

#include <utility>

class FlyerCamera : public Camera
{
public:
    FlyerCamera(void);
    ~FlyerCamera();

    void OnUpdate(void);

private:
    Point m_Position = Point(0.0f, 0.0f, 3.0f);
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;
    Vector m_Up = Vector(0.0f, 1.0f, 0.0f);
    std::pair<int, int> m_PrevMousePosition;

    float m_FieldOfView = 60.0f;
    float m_Near = 0.1f;
    float m_Far = 50.0f;
};

class Simulation : public Application
{
public:
    Simulation(void);
    virtual ~Simulation();

protected:
    virtual void OnInit() override;
    virtual void OnUpdate() override;

private:
    Mesh m_Mesh;
    Shader m_Shader;
    FlyerCamera m_Camera;
};