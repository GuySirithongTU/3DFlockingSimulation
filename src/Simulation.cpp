#include "Simulation.h"

#include "Input.h"
#include "Math.h"
#include "Utility.h"

#include <glad/glad.h>

#pragma region flyer_camera

#define CAM_TURN_SPEED 0.1f
#define CAM_MOVE_SPEED 1.0f
#define CAM_TRACK_POS_OFFSET Vector(0.0f, 2.0f, 6.0f)
#define CAM_TRACK_FOCUS_OFFSET Vector(0.0f, 2.0f, 0.0f)

FlyerCamera::FlyerCamera(void)
{
    m_PrevMousePosition = Input::GetInstance()->GetMousePosition();
    m_Projection = Matrix4::Perspective(m_FieldOfView, Application::GetInstance()->GetWindow()->GetAspect(), m_Near, m_Far);
}

FlyerCamera::~FlyerCamera() {}

void FlyerCamera::OnUpdate(void)
{
    switch(m_Mode)
    {
        case FlyerMode::Ghost:
            UpdateFlyer();
            break;
        case FlyerMode::AlphaTrack:
            UpdateAlphaTrack();
            break;
    }
}

void FlyerCamera::UpdateFlyer(void)
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
}

void FlyerCamera::UpdateAlphaTrack(void)
{
    Point alphaPosition = Simulation::GetInstance()->GetAlphaBoid()->GetPosition();
    float pitch = Simulation::GetInstance()->GetAlphaBoid()->GetPitch();
    float yaw = Simulation::GetInstance()->GetAlphaBoid()->GetYaw();

    Matrix4 orient = Matrix4::RotateY(yaw) * Matrix4::RotateX(pitch);
    Point position = (orient * CAM_TRACK_POS_OFFSET) + alphaPosition;
    Point focus = (orient * CAM_TRACK_FOCUS_OFFSET) + alphaPosition;
    
    m_View = Matrix4::LookAt(position, focus, Vector(0.0f, 1.0f, 0.0f));
}

void FlyerCamera::OnResize(int width, int height)
{
    m_Projection = Matrix4::Perspective(m_FieldOfView, (float)width / (float)height, m_Near, m_Far);
}

void FlyerCamera::SetPosition(const Point& position)
{
    m_Position = position;
}

void FlyerCamera::SetMode(FlyerMode mode)
{
    m_Mode = mode;
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
float Boid::s_AlphaAlignWeight = 0.2f;
float Boid::s_AlphaCohereWeight = 0.2f;

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

    OnPhysicsUpdate();

    Mirror();
}

void Boid::OnDraw(void)
{
    // set material
    s_Mesh.GetShader()->Bind();
    s_Mesh.GetShader()->SetMaterial(*m_Material);
    
    // draw mesh
    Renderer::GetInstance()->DrawMesh(s_Mesh, ComputeModel());
}

Matrix4 Boid::ComputeModel(void) const
{
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
    return model;
}

Point Boid::GetPosition(void) const
{
    return m_Position;
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

void Boid::Steer(const Vector& desired, float weight)
{
    Vector force = desired - m_Velocity;
    if(force.Magnitude() > s_MaxForce) {
        force.Normalize();
        force = s_MaxForce * force * weight;
    }
    AddForce(force);
}

void Boid::Seek(const Point& target, float speed, float weight)
{
    Vector offset = target - m_Position;
    Vector direction = Vector::Normalize(offset);
    float distance = offset.Magnitude();

    Vector desired = speed * direction;
    if(distance < s_ArrivalDistance)
        desired = (distance / s_ArrivalDistance) * desired;
    
    Steer(desired, weight);
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
            desired = Clamp(s_SeparateDistance / distance, 0.0f, s_MaxSpeed) * desired;
            Steer(desired, s_SeparateWeight);
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

    averageForward.Normalize();
    averageForward = s_MaxSpeed * averageForward;
    Steer(averageForward, s_AlignWeight);

    // align alpha
    Steer(Simulation::GetInstance()->GetAlphaBoid()->m_Forward * s_MaxSpeed, s_AlphaAlignWeight);
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

    Seek(Point(averagePosition.x, averagePosition.y, averagePosition.z), s_MaxSpeed, s_CohereWeight);

    // cohere alpha
    Seek(Simulation::GetInstance()->GetAlphaBoid()->m_Position, s_MaxSpeed, s_AlphaCohereWeight);
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

#pragma region alpha_boid

#define ALPHA_BOID_TURN_SPEED 3.0f;

AlphaBoid::AlphaBoid(void) {}

AlphaBoid::~AlphaBoid() {}

void AlphaBoid::OnUpdate(void)
{
    // update pitch and yaw
    if(Input::GetInstance()->GetKey(KEY_UP))
        m_Pitch += ALPHA_BOID_TURN_SPEED;
    if(Input::GetInstance()->GetKey(KEY_DOWN))
        m_Pitch -= ALPHA_BOID_TURN_SPEED;
    if(Input::GetInstance()->GetKey(KEY_LEFT))
        m_Yaw += ALPHA_BOID_TURN_SPEED;
    if(Input::GetInstance()->GetKey(KEY_RIGHT))
        m_Yaw -= ALPHA_BOID_TURN_SPEED;
    m_Pitch = Clamp(m_Pitch, -89.0f, 89.0f);

    m_Forward = Matrix4::RotateY(m_Yaw) * Matrix4::RotateX(m_Pitch) * Vector(0.0f, 0.0f, -1.0f);
    m_Velocity = m_Speed * m_Forward;

    OnPhysicsUpdate();
    
    Mirror();
}

void AlphaBoid::OnDraw(void)
{
    if(!m_IsHighlighted) {
        Boid::OnDraw();
        return;
    }

    // setup stencil to draw one
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    
    // set material
    s_Mesh.GetShader()->Bind();
    s_Mesh.GetShader()->SetMaterial(*m_Material);
    
    // draw mesh
    Matrix4 model = ComputeModel();
    Renderer::GetInstance()->DrawMesh(s_Mesh, model);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    
    Shader *phongShader = s_Mesh.GetShader();
    Shader *highlightShader = Simulation::GetInstance()->GetHighlightShader();
    
    highlightShader->Bind();
    highlightShader->SetUniformVec3("u_Color", { m_HighlightColor.r, m_HighlightColor.g, m_HighlightColor.b });
    s_Mesh.SetShader(highlightShader);
    Renderer::GetInstance()->DrawMesh(s_Mesh, model * Matrix4::Scale(1.3f, 1.3f, 1.3f));
    s_Mesh.SetShader(phongShader);

    glDisable(GL_STENCIL_TEST);
}

float AlphaBoid::GetPitch(void) const
{
    return m_Pitch;
}

float AlphaBoid::GetYaw(void) const
{
    return m_Yaw;
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // init shaders
    m_PhongShader.SetFlags(ShaderFlag::Model | ShaderFlag::NormalMatrix);
    m_PhongShader.InitShader("assets/shaders/Phong_V.glsl", "assets/shaders/Phong_F.glsl");
    m_UnlitShader.SetFlags(ShaderFlag::Model);
    m_UnlitShader.InitShader("assets/shaders/Unlit_V.glsl", "assets/shaders/Unlit_F.glsl");
    m_SkyboxShader.SetFlags(ShaderFlag::NoTranslateView);
    m_SkyboxShader.InitShader("assets/shaders/Skybox_V.glsl", "assets/shaders/Skybox_F.glsl");
    m_SkyboxShader.Bind();
    m_SkyboxShader.SetUniformInt("u_Skybox", 0);
    m_HighlightShader.InitShader("assets/shaders/Highlight_V.glsl", "assets/shaders/Highlight_F.glsl");
    m_HighlightShader.SetFlags(ShaderFlag::Model);
    m_Renderer.AddShader(&m_PhongShader);
    m_Renderer.AddShader(&m_UnlitShader);
    m_Renderer.AddShader(&m_SkyboxShader);
    m_Renderer.AddShader(&m_HighlightShader);
    
    // init camera
    m_Camera.SetPosition({ 0.0f, 0.0f, 60.0f });
    m_Renderer.SetCamera(&m_Camera);
    
    // init light
    m_PhongShader.Bind();
    m_PhongShader.SetDirLight({
        { 0.05f, 0.05f, 0.15f },
        { 0.9f, 0.9f, 0.9f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -2.0f, 2.0f }
    });
    
    // init boid data
    Boid::InitMesh(&m_PhongShader);
    m_BoidMaterial = Material({
        { 0.9f, 0.3f, 0.1f },
        { 0.9f, 0.3f, 0.1f },
        { 1.0f, 1.0f, 1.0f },
        50
    });
    m_AlphaBoidMaterial = Material({
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
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
    m_AlphaBoid.SetMaterial(&m_AlphaBoidMaterial);

    // init bound data
    std::vector<int> layout = { 3 };
    m_BoundMesh.InitData("assets/models/Bound.mesh", layout, GL_LINES, &m_UnlitShader);
    m_UnlitShader.Bind();
    m_UnlitShader.SetUniformVec3("u_Color", Vector(1.0f, 1.0f, 1.0f));

    // init skybox
    const char *skyboxPaths[] = {
        "assets/textures/skybox_right.jpg",     "assets/textures/skybox_left.jpg",
        "assets/textures/skybox_top.jpg",       "assets/textures/skybox_bottom.jpg",
        "assets/textures/skybox_front.jpg",     "assets/textures/skybox_back.jpg",
    };

    m_SkyboxMesh.InitData("assets/models/Skybox.mesh", layout, GL_TRIANGLES, &m_SkyboxShader);
    m_Skybox.Load(0, skyboxPaths);
}

void Simulation::OnUpdate(void)
{
    for(int i = 0; i < BOID_COUNT; i++)
        m_Boids[i].OnUpdate();
    m_AlphaBoid.OnUpdate();
    
    // update camera
    m_Camera.OnUpdate();
}

void Simulation::OnRender(void)
{
    // draw boids
    m_AlphaBoid.OnDraw();
    for(int i = 0; i < BOID_COUNT; i++)
        m_Boids[i].OnDraw();

    // draw bounds
    m_UnlitShader.Bind();
    m_UnlitShader.SetUniformVec3("u_Color", Vector(1.0f, 1.0f, 1.0f));
    m_Renderer.DrawMesh(m_BoundMesh, Matrix4::Scale(BOUND_SIZE, BOUND_SIZE, BOUND_SIZE));
    
    // draw skybox
    glDepthFunc(GL_LEQUAL);
    m_Skybox.Bind(0);
    m_Renderer.DrawMesh(m_SkyboxMesh, Matrix4::Identity());
    glDepthFunc(GL_LESS);
}

void Simulation::OnGUIRender(void)
{
    ImGui::Begin("System");
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    
    ImGui::Begin("Flocking");

    if(ImGui::Checkbox("Track Alpha", &m_TrackingAlpha)) {
        if(m_TrackingAlpha)
            m_Camera.SetMode(FlyerMode::AlphaTrack);
        else
            m_Camera.SetMode(FlyerMode::Ghost);
    }
    ImGui::Checkbox("Alpha Highlight", &m_AlphaBoid.m_IsHighlighted);

    if(ImGui::CollapsingHeader("Limits", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Max Speed", &Boid::s_MaxSpeed, 0.1f, 2.0f);
        ImGui::SliderFloat("Max Force", &Boid::s_MaxForce, 0.01f, 0.2f);
    }

    if(ImGui::CollapsingHeader("Distances", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Arrival Distance",  &Boid::s_ArrivalDistance, 1.0f, 20.0f);
        ImGui::SliderFloat("Separate Distance", &Boid::s_SeparateDistance, 1.0f, 20.0f);
        ImGui::SliderFloat("Align Distance",    &Boid::s_AlignDistance, 1.0f, 20.0f);
        ImGui::SliderFloat("Cohere Distance",   &Boid::s_CohereDistance, 1.0f, 20.0f);
    }
    
    if(ImGui::CollapsingHeader("Weights", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Separate Weight",   &Boid::s_SeparateWeight, 0.0f, 2.0f);
        ImGui::SliderFloat("Align Weight",      &Boid::s_AlignWeight, 0.0f, 2.0f);
        ImGui::SliderFloat("Cohere Weight",     &Boid::s_CohereWeight, 0.0f, 2.0f);
        ImGui::SliderFloat("Alpha Align Weight", &Boid::s_AlphaAlignWeight, 0.0f, 2.0f);
        ImGui::SliderFloat("Alpha Cohere Weight", &Boid::s_AlphaCohereWeight, 0.0f, 2.0f);
    }

    if(ImGui::CollapsingHeader("Colors")) {
        if(ImGui::ColorEdit3("Boid Color", &m_BoidMaterial.diffuse.r))
            m_BoidMaterial.ambient = m_BoidMaterial.diffuse;
        if(ImGui::ColorEdit3("Alpha Boid Color", &m_AlphaBoidMaterial.diffuse.r))
            m_AlphaBoidMaterial.ambient = m_AlphaBoidMaterial.diffuse;
        ImGui::ColorEdit3("Alpha Highlight Color", &m_AlphaBoid.m_HighlightColor.r);
    }

    ImGui::End();
}

void Simulation::OnResize(int width, int height)
{
    m_Camera.OnResize(width, height);
}

Boid *Simulation::GetBoids(void)
{
    return m_Boids;
}

AlphaBoid *Simulation::GetAlphaBoid(void)
{
    return &m_AlphaBoid;
}

Shader *Simulation::GetHighlightShader(void)
{
    return &m_HighlightShader;
}

#pragma endregion