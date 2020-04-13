#pragma once

#include "Application.h"

#include <utility>

#pragma region flyer_camera

class FlyerCamera : public Camera
{
public:
    FlyerCamera(void);
    ~FlyerCamera();

    void OnUpdate(void);

    void SetPosition(const Point& position);

private:
    Point m_Position = Point(0.0f, 0.0f, 3.0f);
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;
    Vector m_Up = Vector(0.0f, 1.0f, 0.0f);

    float m_FieldOfView = 60.0f;
    float m_Near = 0.1f;
    float m_Far = 200.0f;

    std::pair<int, int> m_PrevMousePosition;
    bool m_PrevMouseHold = false;
};

#pragma endregion

#pragma region boid

#define BOID_COUNT 10

class Boid
{
public:
    Boid(void);
    ~Boid();

    static void InitMesh(Shader *shader); 
    void OnUpdate(void);
    void OnDraw(void) const;

    void SetMaterial(const Material *material);

private:
    void OnPhysicsUpdate(void);
    void AddForce(const Vector& force);
    void Steer(const Vector& desired);
    void Seek(const Point& target, float speed);
    void Separate(void);
    void Align(void);
    void Cohere(void);

private:
    Point m_Position;
    Vector m_Velocity;
    Vector m_Acceleration;
    Vector m_Forward = Vector(0.0f, 0.0f, -1.0f);
    float m_Mass = 20.0f;

    static Mesh s_Mesh;
    const Material *m_Material;

    static float s_MaxSpeed;
    static float s_MaxForce;
    static float s_ArrivalDistance;
    static float s_SeparateDistance;
    static float s_AlignDistance;
    static float s_CohereDistance;
    static float s_SeparateWeight;
    static float s_AlignWeight;
    static float s_CohereWeight;
};

#pragma endregion

#pragma region simulation

class Simulation : public Application
{
public:
    static Simulation *GetInstance(void);
private:
    static Simulation *s_Instance;
public:
    Simulation(void);
    virtual ~Simulation();

    Boid *GetBoids(void);

protected:
    virtual void OnInit() override;
    virtual void OnUpdate() override;

private:
    Shader m_PhongShader;
    Shader m_UnlitShader;
    Material m_BoidMaterial;
    FlyerCamera m_Camera;

    Boid m_Boids[BOID_COUNT];
    Mesh m_BoundMesh;
};

#pragma endregion