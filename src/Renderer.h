#pragma once

#include "RenderingPrimitives.h"
#include "Math.h"

class Camera
{
public:
    Camera(void);
    ~Camera();

    Matrix4 GetView(void) const;
    Matrix4 GetNoTranslateView(void) const;
    Matrix4 GetProjection(void) const;
    void SetView(const Matrix4& view);
    void SetProjection(const Matrix4& projection);

protected:
    Matrix4 m_View;
    Matrix4 m_Projection;
};

class Renderer
{
public:
    static Renderer *GetInstance();
private:
    static Renderer *s_Instance;

public:
    Renderer(void);
    ~Renderer();

    void BeginScene(void);
    void DrawMesh(const Mesh &mesh, const Matrix4& model);
    void Clear(const Color& color);

    void SetCamera(Camera *camera);
    void AddShader(Shader *shader);

private:
    Camera *m_Camera;
    std::vector<Shader *> m_Shaders;
};