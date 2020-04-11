#include "Simulation.h"

#include "Input.h"
#include "Math.h"
#include "Utility.h"

#define CAM_TURN_SPEED 0.1f
#define CAM_MOVE_SPEED 0.1f

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
    if(Input::GetInstance()->GetMouseButton(MOUSE_BUTTON_2)) {
        m_Pitch -= cursorMove.second * CAM_TURN_SPEED;
        m_Yaw -= cursorMove.first * CAM_TURN_SPEED;
        m_Pitch = Clamp(m_Pitch, -179.0f, 179.0f);
    }
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

Simulation::Simulation(void) {}
Simulation::~Simulation(void) {}

void Simulation::OnInit(void)
{
    float vertices[] = {
        -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f
    };

    std::vector<int> layout = { 3, 3 };

    m_Mesh.InitData(vertices, sizeof(vertices), layout);
    m_Shader.InitShader("assets/shaders/Phong_V.glsl", "assets/shaders/Phong_F.glsl");

    m_Renderer.SetShader(&m_Shader);
    m_Renderer.SetCamera(&m_Camera);
}

void Simulation::OnUpdate(void)
{
    m_Camera.OnUpdate();
    m_Renderer.BeginScene();
    m_Renderer.Clear({ 1.0f, 0.0f, 1.0f, 1.0f });
    m_Renderer.DrawMesh(m_Mesh, Matrix4::Identity());
}