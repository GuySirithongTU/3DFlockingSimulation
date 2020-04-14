#pragma once

#include "Application.h"

#include <utility>

#pragma region flyer_camera

enum class FlyerMode
{
    Ghost = 0,
    AlphaTrack
};

class FlyerCamera : public Camera
{
public:
    FlyerCamera(void);
    ~FlyerCamera();

    void OnUpdate(void);
    void OnResize(int width, int height);

    void SetPosition(const Point& position);
    void SetMode(FlyerMode mode);

private:
    void UpdateFlyer(void);
    void UpdateAlphaTrack(void);

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

    FlyerMode m_Mode = FlyerMode::Ghost;
};

#pragma endregion

#pragma region boid

#define BOUND_SIZE 50.0f
#define BOID_COUNT 100

class Boid
{
public:
    Boid(void);
    ~Boid();

    static void InitMesh(Shader *shader); 
    virtual void OnUpdate(void);
    void OnDraw(void) const;

    Point GetPosition(void) const;
    void SetPosition(const Point& position);
    void SetVelocity(const Vector& velocity);
    void SetMaterial(const Material *material);
    static float GetMaxSpeed(void);

protected:
    void OnPhysicsUpdate(void);
    void AddForce(const Vector& force);
    void Steer(const Vector& desired, float weight);
    void Seek(const Point& target, float speed, float weight);
    void Separate(void);
    void Align(void);
    void Cohere(void);
    void Mirror(void);

protected:
    Point m_Position;
    Vector m_Velocity;
    Vector m_Acceleration;
    Vector m_Forward = Vector(0.0f, 0.0f, -1.0f);
    float m_Mass = 20.0f;

    static Mesh s_Mesh;
    const Material *m_Material;

    static float s_MaxSpeed, s_MaxForce;
    static float s_ArrivalDistance, s_SeparateDistance, s_AlignDistance, s_CohereDistance;
    static float s_SeparateWeight, s_AlignWeight, s_CohereWeight, s_AlphaAlignWeight, s_AlphaCohereWeight;

private:
    friend class Simulation;
};

#pragma endregion

#pragma region alpha_boid

class AlphaBoid : public Boid
{
public:
    AlphaBoid(void);
    ~AlphaBoid();

    virtual void OnUpdate(void) override;

    float GetPitch(void) const;
    float GetYaw(void) const;

private:
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;
    float m_Speed = 0.4f;
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
    AlphaBoid *GetAlphaBoid(void);

protected:
    virtual void OnInit(void) override;
    virtual void OnUpdate(void) override;
    virtual void OnRender(void) override;
    virtual void OnGUIRender(void) override;
    virtual void OnResize(int width, int height) override;

private:
    // shaders
    Shader m_PhongShader;
    Shader m_UnlitShader;
    Shader m_SkyboxShader;

    FlyerCamera m_Camera;

    // boids
    Material m_BoidMaterial;
    Material m_AlphaBoidMaterial;
    Boid m_Boids[BOID_COUNT];
    AlphaBoid m_AlphaBoid;
    
    // bound and skybox
    Mesh m_BoundMesh;
    Mesh m_SkyboxMesh;
    CubeMap m_Skybox;

    bool m_TrackingAlpha = false;
};

#pragma endregion