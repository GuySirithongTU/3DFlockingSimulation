#include "Renderer.h"

#include <iostream>

#include <glad/glad.h>

Camera::Camera(void) {}
Camera::~Camera() {}

Matrix4 Camera::GetView(void) const { return m_View; }
Matrix4 Camera::GetProjection(void) const { return m_Projection; }
void Camera::SetView(const Matrix4& view) { m_View = view; }
void Camera::SetProjection(const Matrix4& projection) { m_Projection = projection; }

Renderer *Renderer::s_Instance = nullptr;
Renderer *Renderer::GetInstance()
{
    return s_Instance;
}

Renderer::Renderer(void) {}
Renderer::~Renderer() {}

void Renderer::BeginScene(void)
{
    SetUniformMat4("u_View", m_Camera->GetView());
    SetUniformMat4("u_Projection", m_Camera->GetProjection());
}

void Renderer::DrawMesh(const Mesh& mesh, const Matrix4& model)
{
    if(m_Shader == nullptr) {
        std::cout << "RENDERER::ERROR: drawing mesh when shader is not binded" << std::endl;
        return;
    }

    m_Shader->Bind();
    mesh.Bind();
    m_Shader->SetUniformMat4("u_Model", model);
    glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
    
    mesh.Unbind();
}

void Renderer::Clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#pragma region setters

void Renderer::SetCamera(Camera *camera)
{
    m_Camera = camera;
}

void Renderer::SetShader(Shader *shader)
{
    m_Shader = shader;
}

void Renderer::SetUniformInt(const char *name, int value)
{
    if(m_Shader == nullptr)
        std::cout << "RENDERER::ERROR: drawing mesh when shader is not binded" << std::endl;
    else {
        m_Shader->Bind();
        m_Shader->SetUniformInt(name, value);
    }
}

void Renderer::SetUniformFloat(const char *name, float value)
{
    if(m_Shader == nullptr)
        std::cout << "RENDERER::ERROR: drawing mesh when shader is not binded" << std::endl;
    else {
        m_Shader->Bind();
        m_Shader->SetUniformFloat(name, value);
    }
}

void Renderer::SetUniformVec3(const char *name, const Vector& value)
{
    if(m_Shader == nullptr)
        std::cout << "RENDERER::ERROR: drawing mesh when shader is not binded" << std::endl;
    else {
        m_Shader->Bind();
        m_Shader->SetUniformVec3(name, value);
    }
}

void Renderer::SetUniformVec4(const char *name, const Vector& value)
{
    if(m_Shader == nullptr)
        std::cout << "RENDERER::ERROR: drawing mesh when shader is not binded" << std::endl;
    else {
        m_Shader->Bind();
        m_Shader->SetUniformVec4(name, value);
    }
}

void Renderer::SetUniformMat3(const char *name, const Matrix3& value)
{
    if(m_Shader == nullptr)
        std::cout << "RENDERER::ERROR: drawing mesh when shader is not binded" << std::endl;
    else {
        m_Shader->Bind();
        m_Shader->SetUniformMat3(name, value);
    }
}

void Renderer::SetUniformMat4(const char *name, const Matrix4& value)
{
    if(m_Shader == nullptr)
        std::cout << "RENDERER::ERROR: drawing mesh when shader is not binded" << std::endl;
    else {
        m_Shader->Bind();
        m_Shader->SetUniformMat4(name, value);
    }
}

#pragma endregion