#include "Renderer.h"

#include <iostream>

#include <glad/glad.h>

Camera::Camera(void) {}
Camera::~Camera() {}

Matrix4 Camera::GetView(void) const { return m_View; }
Matrix4 Camera::GetNoTranslateView(void) const
{
    Matrix4 view = m_View;
    view[0][3] = 0.0f;
    view[1][3] = 0.0f;
    view[2][3] = 0.0f;
    return view;
}
Matrix4 Camera::GetProjection(void) const { return m_Projection; }
void Camera::SetView(const Matrix4& view) { m_View = view; }
void Camera::SetProjection(const Matrix4& projection) { m_Projection = projection; }

Renderer *Renderer::s_Instance = nullptr;
Renderer *Renderer::GetInstance()
{
    return s_Instance;
}

Renderer::Renderer(void)
{
    if(s_Instance == nullptr) {
        s_Instance = this;
    } else {
        std::cout << "RENDERER::ERROR: creating second renderer" << std::endl;
        return;
    }
}

Renderer::~Renderer() {}

void Renderer::BeginScene(void)
{
    for(std::vector<Shader *>::iterator it = m_Shaders.begin(); it != m_Shaders.end(); it++) {
        (*it)->Bind();
        if((*it)->GetFlag(ShaderFlag::NoTranslateView))
            (*it)->SetUniformMat4("u_View", m_Camera->GetNoTranslateView());
        else
            (*it)->SetUniformMat4("u_View", m_Camera->GetView());
        (*it)->SetUniformMat4("u_Projection", m_Camera->GetProjection());
    }
}

void Renderer::DrawMesh(const Mesh& mesh, const Matrix4& model)
{
    mesh.GetShader()->Bind();
    mesh.Bind();
    if(mesh.GetShader()->GetFlag(ShaderFlag::Model))
        mesh.GetShader()->SetUniformMat4("u_Model", model);
    if(mesh.GetShader()->GetFlag(ShaderFlag::NormalMatrix))
        mesh.GetShader()->SetUniformMat4("u_NormalMat", Matrix4::Transpose(Matrix4::Invert(m_Camera->GetView() * model)));
    glDrawArrays(mesh.GetMode(), 0, mesh.GetVertexCount());
    
    mesh.Unbind();
}

void Renderer::Clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::AddShader(Shader *shader)
{
    m_Shaders.push_back(shader);
}

#pragma region setters

void Renderer::SetCamera(Camera *camera)
{
    m_Camera = camera;
}

#pragma endregion