#pragma once

#include "Math.h"

#include <vector>

#pragma region primitive

class Primitive
{
public:
    Primitive(void);
    virtual ~Primitive();

    virtual void Bind(void) const = 0;
    virtual void Unbind(void) const = 0;

    int GetCount(void) const;

protected:
    void GenerateBuffer(void);
    void DeleteBuffer(void);

protected:
    unsigned int m_Id = -1;
    bool m_Initialized = false;
    int m_Count = 0;
};

#pragma endregion

#pragma region buffers

class VertexBuffer : public Primitive
{
public:
    VertexBuffer(void);
    virtual ~VertexBuffer();

    virtual void Bind(void) const override;
    virtual void Unbind(void) const override;

    void BufferData(float *data, int vertexCount, const std::vector<int>& layout);
};

#pragma endregion

#pragma region vertex_array

class VertexArray : public Primitive
{
public:
    VertexArray(void);
    ~VertexArray();

    virtual void Bind(void) const override;
    virtual void Unbind(void) const override;

    void Init(void);
};

#pragma endregion

#pragma region mesh

class Shader;
class Mesh
{
public:
    Mesh(void);
    ~Mesh();

    void Bind(void) const;
    void Unbind(void) const;

    void InitData(const char *path, const std::vector<int>& layout, unsigned int mode, Shader *shader);
    void InitData(float *vertices, int vertexCount, const std::vector<int>& layout, unsigned int mode, Shader *shader);

    unsigned int GetMode(void) const;
    Shader *GetShader(void) const;
    int GetVertexCount(void) const;

private:
    VertexBuffer m_VertexBuffer;
    VertexArray m_VertexArray;
    unsigned int m_Mode;
    Shader *m_Shader;
};

#pragma endregion

#pragma region shader

struct Material
{
    Color ambient;
    Color diffuse;
    Color specular;
    int shininess;

    bool operator==(const Material& rhs) const;
};

struct DirLight
{
    Color ambient;
    Color diffuse;
    Color specular;
    Vector direction;
};

class Shader : public Primitive
{
public:
    Shader(void);
    ~Shader();

    virtual void Bind(void) const override;
    virtual void Unbind(void) const override;

    void InitShader(const char *vertexPath, const char *fragmentPath);

    int GetUniformLocation(const char *name) const;
    void SetUniformInt(const char *name, int value);
    void SetUniformFloat(const char *name, float value);
    void SetUniformVec3(const char *name, const Vector& value);
    void SetUniformVec4(const char *name, const Vector& value);
    void SetUniformMat3(const char *name, const Matrix3& value);
    void SetUniformMat4(const char *name, const Matrix4& value);

    bool IsEnableNormalMatrixUniform(void) const;
    void SetEnableNormalMatrixUniform(bool enabled);
    void SetMaterial(const Material& material);
    void SetDirLight(const DirLight& light);

private:
    unsigned int CompileShader(const char *path, unsigned int type);
    Material m_CurrentMaterial = { Color(), Color(), Color(), 0 };
    bool m_NormalMatrixEnabled = false;
};

#pragma endregion