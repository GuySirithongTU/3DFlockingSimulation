#include "RenderingPrimitives.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>

#include <glad/glad.h>
#include <stb_image.h>

#pragma region primitive

Primitive::Primitive(void) {}
Primitive::~Primitive() {}

void Primitive::GenerateBuffer(void)
{
    glGenBuffers(1, &m_Id);
}

void Primitive::DeleteBuffer(void)
{
    glDeleteBuffers(1, &m_Id);
}

int Primitive::GetCount(void) const
{
    return m_Count;
}

#pragma endregion

#pragma region buffers

VertexBuffer::VertexBuffer(void) {}
VertexBuffer::~VertexBuffer()
{
    if(m_Initialized)
        DeleteBuffer();
}

void VertexBuffer::Bind(void) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void VertexBuffer::Unbind(void) const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::BufferData(float *data, int vertexCount, const std::vector<int>& layout)
{
    // buffer data
    int stride = std::accumulate(layout.begin(), layout.end(), 0);
    GenerateBuffer();
    Bind();
    glBufferData(GL_ARRAY_BUFFER, stride * vertexCount * sizeof(float), (void *)data, GL_STATIC_DRAW);

    // set layout
    int offset = 0;
    for(int i = 0; i < layout.size(); i++) {
        glVertexAttribPointer(i, layout[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);
        offset += layout[i];
    }

    m_Count = vertexCount;
    m_Initialized = true;
}

#pragma endregion

#pragma region vertex_array

VertexArray::VertexArray(void) {}
VertexArray::~VertexArray()
{
    if(m_Initialized)
        glDeleteVertexArrays(1, &m_Id);
}

void VertexArray::Init(void)
{
    glGenVertexArrays(1, &m_Id);
    m_Initialized = true;
}

void VertexArray::Bind(void) const
{
    glBindVertexArray(m_Id);
}

void VertexArray::Unbind(void) const
{
    glBindVertexArray(0);
}

#pragma endregion

#pragma region mesh

Mesh::Mesh(void) {}
Mesh::~Mesh() {}

void Mesh::Bind(void) const
{
    m_VertexArray.Bind();
}

void Mesh::Unbind(void) const
{
    m_VertexArray.Unbind();
}

void Mesh::InitData(const char *path, const std::vector<int>& layout, unsigned int mode, Shader *shader)
{
    // open file
    std::ifstream file(path);
    if(!file.is_open()) {
        std::cout << "MESH::ERROR: failed to open file: " << path << std::endl;
        return;
    }

    int attributeCount, vertexCount, length;
    std::vector<float> vertices;
    float attribute;

    // read vertices
    attributeCount = std::accumulate(layout.begin(), layout.end(), 0);
    while(!file.eof()) {
        file >> attribute;
        vertices.push_back(attribute);
        length++;
    }
    file.close();
    vertexCount = length / attributeCount;

    InitData(vertices.data(), vertexCount, layout, mode, shader);
}

void Mesh::InitData(float *vertices, int vertexCount, const std::vector<int>& layout, unsigned int mode, Shader *shader)
{
    m_Mode = mode;
    m_Shader = shader;
    m_VertexArray.Init();
    m_VertexArray.Bind();
    m_VertexBuffer.BufferData(vertices, vertexCount, layout);
    m_VertexArray.Unbind();
}

unsigned int Mesh::GetMode(void) const
{
    return m_Mode;
}

Shader *Mesh::GetShader(void) const
{
    return m_Shader;
}

int Mesh::GetVertexCount(void) const
{
    return m_VertexBuffer.GetCount();
}

void Mesh::SetShader(Shader *shader)
{
    m_Shader = shader;
}

#pragma endregion

#pragma region cube_map

CubeMap::CubeMap(void) {}

CubeMap::~CubeMap()
{
    if(m_Initialized)
        glDeleteTextures(1, &m_Id);
}

void CubeMap::Bind(void) const
{
    Bind(0);
}

void CubeMap::Unbind(void) const
{
    Unbind(0);
}

void CubeMap::Bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);
}

void CubeMap::Unbind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::Load(unsigned int unit, const char *path[])
{
    glGenTextures(1, &m_Id);
    Bind(unit);

    int width, height, channels;
    unsigned char *data;

    for(int i = 0; i < 6; i++) {
        data = stbi_load(path[i], &width, &height, &channels, 0);
        if(channels == 0) {
            std::cout << "CUBEMAP::ERROR: failed to load texture: " << path[i];
            return;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

#pragma endregion

#pragma region shader

bool Material::operator==(const Material& rhs) const
{
    return  ambient == rhs.ambient &&
            diffuse == rhs.diffuse &&
            specular == rhs.specular &&
            shininess == rhs.shininess;
}

Shader::Shader(unsigned char flags)
    : m_Flags(flags) {}

Shader::~Shader()
{
    if(m_Initialized)
        glDeleteProgram(m_Id);
}

void Shader::Bind(void) const
{
    glUseProgram(m_Id);
}

void Shader::Unbind(void) const
{
    glUseProgram(0);
}

#define SHADER_INFO_LOG_BUF_SIZE 512
void Shader::InitShader(const char *vertexPath, const char *fragmentPath)
{
    // compile shaders
    unsigned int vertex = CompileShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragment = CompileShader(fragmentPath, GL_FRAGMENT_SHADER);
    if(vertex == -1 || fragment == -1)
        return;
    
    // gen program
    m_Id = glCreateProgram();
    glAttachShader(m_Id, vertex);
    glAttachShader(m_Id, fragment);
    glLinkProgram(m_Id);

    // check error
    int success;
    char infoLog[SHADER_INFO_LOG_BUF_SIZE];
    glGetProgramiv(m_Id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(m_Id, SHADER_INFO_LOG_BUF_SIZE, NULL, infoLog);
        return;
    }

    // delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

unsigned int Shader::CompileShader(const char *path, unsigned int type)
{
    // read source from file
    std::ifstream file(path);
    if(!file.is_open()) {
        std::cout << "SHADER::ERROR: failed to load shader source from file: " << path << std::endl;
        return -1;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    std::string str = ss.str();
    const char *source = str.c_str();

    // gen shader
    unsigned int shader = glCreateShader(type);
    int success;
    char infoLog[SHADER_INFO_LOG_BUF_SIZE];
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // check error
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, SHADER_INFO_LOG_BUF_SIZE, NULL, infoLog);
        std::cout << "SHADER::ERROR: compilation error: " << path << std::endl << infoLog << std::endl;
        return -1;
    }

    return shader;
}

int Shader::GetUniformLocation(const char *name) const
{
    return glGetUniformLocation(m_Id, name);
}

void Shader::SetUniformInt(const char *name, int value)
{
    int location = GetUniformLocation(name);
    if(location >= 0)
        glUniform1i(location, value);
    else
        std::cout << "SHADER::ERROR: uniform not found: " << name << std::endl;
}

void Shader::SetUniformFloat(const char *name, float value)
{
    int location = GetUniformLocation(name);
    if(location >= 0)
        glUniform1f(location, value);
    else
        std::cout << "SHADER::ERROR: uniform not found: " << name << std::endl;
}

void Shader::SetUniformVec3(const char *name, const Vector& value)
{
    int location = GetUniformLocation(name);
    if(location >= 0)
        glUniform3fv(location, 1, &value.x);
    else
        std::cout << "SHADER::ERROR: uniform not found: " << name << std::endl;
}

void Shader::SetUniformVec4(const char *name, const Vector& value)
{
    int location = GetUniformLocation(name);
    if(location >= 0)
        glUniform4fv(location, 1, &value.x);
    else
        std::cout << "SHADER::ERROR: uniform not found: " << name << std::endl;
}

void Shader::SetUniformMat3(const char *name, const Matrix3& value)
{
    int location = GetUniformLocation(name);
    if(location >= 0)
        glUniformMatrix3fv(location, 1, true, value.GetData());
    else
        std::cout << "SHADER::ERROR: uniform not found: " << name << std::endl;
}

void Shader::SetUniformMat4(const char *name, const Matrix4& value)
{
    int location = GetUniformLocation(name);
    if(location >= 0) {
        glUniformMatrix4fv(location, 1, true, value.GetData());
    }
    else
        std::cout << "SHADER::ERROR: uniform not found: " << name << std::endl;
}

bool Shader::GetFlag(ShaderFlag flag) const
{
    return m_Flags & flag;
}

void Shader::SetFlag(ShaderFlag flag, bool value)
{
    if(value)
        m_Flags |= flag;
    else
        m_Flags &= ~flag;
}

unsigned char Shader::GetFlags(void) const
{
    return m_Flags;
}

void Shader::SetFlags(unsigned char flags)
{
    m_Flags = flags;
}

void Shader::SetMaterial(const Material& material)
{
    //if(!(material == m_CurrentMaterial)) {
        SetUniformVec3("u_Material.ambient", Vector(material.ambient.r, material.ambient.g, material.ambient.b));
        SetUniformVec3("u_Material.diffuse", Vector(material.diffuse.r, material.diffuse.g, material.diffuse.b));
        SetUniformVec3("u_Material.specular", Vector(material.specular.r, material.specular.g, material.specular.b));
        SetUniformInt("u_Material.shininess", material.shininess);
        m_CurrentMaterial = material;
    //}
}

void Shader::SetDirLight(const DirLight& light)
{
    SetUniformVec3("u_Light.ambient", Vector(light.ambient.r, light.ambient.g, light.ambient.b));
    SetUniformVec3("u_Light.diffuse", Vector(light.diffuse.r, light.diffuse.g, light.diffuse.b));
    SetUniformVec3("u_Light.specular", Vector(light.specular.r, light.specular.g, light.specular.b));
    SetUniformVec3("u_Light.direction", light.direction);
}

#pragma endregion