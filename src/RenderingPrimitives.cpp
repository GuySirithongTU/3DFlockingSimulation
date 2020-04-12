#include "RenderingPrimitives.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>

#include <glad/glad.h>

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

void Mesh::InitData(const char *path, const std::vector<int>& layout)
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

    InitData(vertices.data(), vertexCount, layout);
}

void Mesh::InitData(float *vertices, int vertexCount, const std::vector<int>& layout)
{
    m_VertexArray.Init();
    m_VertexArray.Bind();
    m_VertexBuffer.BufferData(vertices, vertexCount, layout);
    m_VertexArray.Unbind();
}

int Mesh::GetVertexCount(void) const
{
    return m_VertexBuffer.GetCount();
}

#pragma endregion

#pragma region shader

Shader::Shader(void) {}
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

#pragma endregion