#include "Simulation.h"

#include "Input.h"
#include "Math.h"
#include "Utility.h"

#include <glad/glad.h>

#pragma region flyer_camera

#define CAM_TURN_SPEED 0.1f
#define CAM_MOVE_SPEED 1.0f

FlyerCamera::FlyerCamera(void)
{
    m_PrevMousePosition = Input::GetInstance()->GetMousePosition();
}

FlyerCamera::~FlyerCamera() {}

void FlyerCamera::OnUpdate(void)
{
    // get mouse move
    std::pair<int, int> cursorPosition = Input::GetInstance()->GetMousePosition();
    std::pair<int, int> cursorMove = cursorPosition;
    cursorMove.first -= m_PrevMousePosition.first;
    cursorMove.second -= m_PrevMousePosition.second;
    m_PrevMousePosition = cursorPosition;

    // update pitch and yaw
    bool mouseHold = Input::GetInstance()->GetMouseButton(MOUSE_BUTTON_2);
    if(mouseHold) {
        if(!m_PrevMouseHold)
            glfwSetInputMode(Application::GetInstance()->GetWindow()->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        m_Pitch -= cursorMove.second * CAM_TURN_SPEED;
        m_Yaw -= cursorMove.first * CAM_TURN_SPEED;
        m_Pitch = Clamp(m_Pitch, -179.0f, 179.0f);
    } else if(m_PrevMouseHold) {
        Window *window = Application::GetInstance()->GetWindow();
        glfwSetInputMode(window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPos(window->GetGLFWWindow(), window->GetWidth() / 2, window->GetHeight() / 2);        

    }
    m_PrevMouseHold = mouseHold;
    Vector forward = Matrix4::RotateY(m_Yaw) * Matrix4::RotateX(m_Pitch) * Vector(0.0f, 0.0f, -1.0f);

    // update position
    Vector right = Vector::Cross(forward, m_Up);
    if(Input::GetInstance()->GetKey(KEY_W))
        m_Position = m_Position + CAM_MOVE_SPEED * forward;
    if(Input::GetInstance()->GetKey(KEY_S))
        m_Position = m_Position - CAM_MOVE_SPEED * forward;
    if(Input::GetInstance()->GetKey(KEY_D))
        m_Position = m_Position + CAM_MOVE_SPEED * right;
    if(Input::GetInstance()->GetKey(KEY_A))
        m_Position = m_Position - CAM_MOVE_SPEED * right;
    if(Input::GetInstance()->GetKey(KEY_SPACE))
        m_Position = m_Position + CAM_MOVE_SPEED * m_Up;
    if(Input::GetInstance()->GetKey(KEY_LEFT_SHIFT))
        m_Position = m_Position - CAM_MOVE_SPEED * m_Up;

    m_View = Matrix4::LookAt(m_Position, m_Position + forward, m_Up);
    m_Projection = Matrix4::Perspective(m_FieldOfView, Application::GetInstance()->GetWindow()->GetAspect(), m_Near, m_Far);
}

void FlyerCamera::SetPosition(const Point& position)
{
    m_Position = position;
}

#pragma endregion

#pragma region boid

Mesh Boid::s_Mesh;

float Boid::s_MaxSpeed = 0.8f;
float Boid::s_MaxForce = 0.1f;
float Boid::s_ArrivalDistance = 5.0f;
float Boid::s_SeparateDistance = 2.0f;
float Boid::s_AlignDistance = 10.0f;
float Boid::s_CohereDistance = 10.0f;
float Boid::s_SeparateWeight = 1.0f;
float Boid::s_AlignWeight = 1.0f;
float Boid::s_CohereWeight = 1.0f;

Boid::Boid(void) {}

Boid::~Boid() {}

void Boid::InitMesh(Shader *shader)
{
    std::vector<int> layout = { 3, 3 };
    s_Mesh.InitData("assets/models/Boid.mesh", layout, GL_TRIANGLES, shader);
}

void Boid::OnUpdate(void)
{
    Separate();
    Align();
    Cohere();

    Mirror();
    
    OnPhysicsUpdate();
}

void Boid::OnDraw(void) const
{
    // compute model
    float pitch, yaw;
    Vector forward = m_Forward;
    pitch = RAD_TO_DEG(-asin(Tuple::Dot(forward, Vector(0.0f, -1.0f, 0.0f))));
    forward.y = 0.0f;
    forward.Normalize();
    yaw = RAD_TO_DEG(acos(Tuple::Dot(forward, Vector(0.0f, 0.0f, -1.0f))));
    if(Tuple::Dot(forward, Vector(1.0f, 0.0f, 0.0f)) > 0.0f)
        yaw = 360.0f - yaw;
    Matrix4 model = Matrix4::Translate(m_Position.x, m_Position.y, m_Position.z) *
                    Matrix4::RotateY(yaw) * Matrix4::RotateX(pitch);

    // set material
    s_Mesh.GetShader()->Bind();
    s_Mesh.GetShader()->SetMaterial(*m_Material);
    
    // draw mesh
    Renderer::GetInstance()->DrawMesh(s_Mesh, model);
}

void Boid::SetPosition(const Point& position)
{
    m_Position = position;
}

void Boid::SetVelocity(const Vector& velocity)
{
    m_Velocity = velocity;
}

void Boid::SetMaterial(const Material *material)
{
    m_Material = material;
}

float Boid::GetMaxSpeed(void)
{
    return s_MaxSpeed;
}

void Boid::OnPhysicsUpdate(void)
{
    // update velocity
    m_Velocity = m_Velocity + m_Acceleration;
    if(m_Velocity.Magnitude() > s_MaxSpeed) {
        m_Velocity.Normalize();
        m_Velocity = s_MaxSpeed * m_Velocity;
    }

    // update forward
    m_Forward = Vector::Normalize(m_Velocity);

    // translate
    m_Position = m_Position + m_Velocity;

    // reset acceleration
    m_Acceleration = Vector();
}

void Boid::AddForce(const Vector& force)
{
    m_Acceleration = m_Acceleration + (1.0f / m_Mass) * force;
}

void Boid::Steer(const Vector& desired)
{
    Vector force = desired - m_Velocity;
    if(force.Magnitude() > s_MaxForce) {
        force.Normalize();
        force = s_MaxForce * force;
    }
    AddForce(force);
}

void Boid::Seek(const Point& target, float speed)
{
    Vector offset = target - m_Position;
    Vector direction = Vector::Normalize(offset);
    float distance = offset.Magnitude();

    Vector desired = speed * direction;
    if(distance < s_ArrivalDistance)
        desired = (distance / s_ArrivalDistance) * desired;
    
    Steer(desired);
}

void Boid::Separate(void)
{
    Boid *boids = Simulation::GetInstance()->GetBoids();

    for(int i = 0; i < BOID_COUNT; i++) {
        if(boids + i == this)
            continue;

        Vector offset = m_Position - boids[i].m_Position;
        float distance = offset.Magnitude();

        if(distance <= s_SeparateDistance) {
            Vector desired = Vector::Normalize(offset);
            desired = Clamp(s_SeparateDistance / distance, 0.0f, s_MaxSpeed) * s_SeparateWeight * desired;
            Steer(desired);
        }
    }
}

void Boid::Align(void)
{
    Boid *boids = Simulation::GetInstance()->GetBoids();
    
    Vector averageForward;

    for(int i = 0; i < BOID_COUNT; i++) {
        if(boids + i == this)
            continue;
        
        float distance = ((Vector)(m_Position - boids[i].m_Position)).Magnitude();

        if(distance <= s_AlignDistance)
            averageForward = averageForward + boids[i].m_Forward;
    }

    // TODO
    averageForward.Normalize();
    averageForward = s_MaxSpeed * s_AlignWeight * averageForward;
    Steer(averageForward);
}

void Boid::Cohere(void)
{
    Boid *boids = Simulation::GetInstance()->GetBoids();

    Vector averagePosition;
    int count = 0;

    for(int i = 0; i < BOID_COUNT; i++) {
        if(boids + i == this)
            continue;
        
        float distance = ((Vector)(m_Position - boids[i].m_Position)).Magnitude();

        if(distance <= s_CohereDistance) {
            averagePosition = averagePosition + boids[i].m_Position;
            count++;
        }
    }

    if(count > 1)
        averagePosition = (1.0f / count) * averagePosition;

    Seek(averagePosition, s_MaxSpeed * s_CohereWeight);
}

void Boid::Mirror(void)
{
    float radius = BOUND_SIZE * 0.5f;
    
    Point mirrored = m_Position;

    if(m_Position.x > radius)       mirrored.x = -2.0f * radius + m_Position.x;
    else if(m_Position.x < -radius) mirrored.x =  2.0f * radius + m_Position.x;
    if(m_Position.y > radius)       mirrored.y = -2.0f * radius + m_Position.y;
    else if(m_Position.y < -radius) mirrored.y =  2.0f * radius + m_Position.y;
    if(m_Position.z > radius)       mirrored.z = -2.0f * radius + m_Position.z;
    else if(m_Position.z < -radius) mirrored.z =  2.0f * radius + m_Position.z;
    
    m_Position = mirrored;
}

#pragma endregion

#pragma region simulation

Simulation *Simulation::s_Instance = nullptr;
Simulation *Simulation::GetInstance(void)
{
    return s_Instance;
}

Simulation::Simulation(void)
{
    if(s_Instance == nullptr) {
        s_Instance = this;
    } else {
        std::cout << "creating second simulation" << std::endl;
        return;
    }
    
}

Simulation::~Simulation() {}

void Simulation::OnInit(void)
{
    glEnable(GL_DEPTH_TEST);

    // init shaders
    m_PhongShader.InitShader("assets/shaders/Phong_V.glsl", "assets/shaders/Phong_F.glsl");
    m_PhongShader.SetEnableNormalMatrixUniform(true);
    m_UnlitShader.InitShader("assets/shaders/Unlit_V.glsl", "assets/shaders/Unlit_F.glsl");
    m_Renderer.AddShader(&m_PhongShader);
    m_Renderer.AddShader(&m_UnlitShader);
    
    // init camera
    m_Camera.SetPosition({ 0.0f, 0.0f, 60.0f });
    m_Renderer.SetCamera(&m_Camera);
    
    // init light
    m_PhongShader.Bind();
    m_PhongShader.SetDirLight({
        { 0.05f, 0.05f, 0.15f },
        { 0.7f, 0.7f, 0.7f },
        { 0.8f, 0.8f, 0.8f },
        { -1.0f, -3.0f, -1.0f }
    });
    
    // init boid data
    Boid::InitMesh(&m_PhongShader);
    m_BoidMaterial = Material({
        { 0.2f, 0.6f, 0.7f },
        { 0.2f, 0.6f, 0.7f },
        { 1.0f, 1.0f, 1.0f },
        50
    });
    for(int i = 0; i < BOID_COUNT; i++) {
        m_Boids[i].SetMaterial(&m_BoidMaterial);
        m_Boids[i].SetPosition(Vector(
            Random(-BOUND_SIZE / 2.0f, BOUND_SIZE / 2.0f),
            Random(-BOUND_SIZE / 2.0f, BOUND_SIZE / 2.0f),
            Random(-BOUND_SIZE / 2.0f, BOUND_SIZE / 2.0f)));
        m_Boids[i].SetVelocity(Boid::GetMaxSpeed() * RandomUnitSphere());
    }

    // init bound data
    std::vector<int> layout = { 3 };
    m_BoundMesh.InitData("assets/models/Bound.mesh", layout, GL_LINES, &m_UnlitShader);
    m_UnlitShader.Bind();
    m_UnlitShader.SetUniformVec3("u_Color", Vector(1.0f, 1.0f, 1.0f));
}

void Simulation::OnUpdate(void)
{
    m_Camera.OnUpdate();
    m_Renderer.BeginScene();
    m_Renderer.Clear({ 1.0f, 0.0f, 1.0f, 1.0f });
    
    for(int i = 0; i < BOID_COUNT; i++)
        m_Boids[i].OnUpdate();
    
    for(int i = 0; i < BOID_COUNT; i++)
        m_Boids[i].OnDraw();
    
    m_Renderer.DrawMesh(m_BoundMesh, Matrix4::Scale(BOUND_SIZE, BOUND_SIZE, BOUND_SIZE));
}

Boid *Simulation::GetBoids(void)
{
    return m_Boids;
}

#pragma endregion