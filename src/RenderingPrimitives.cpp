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

#pragma endregion

#pragma region buffers

VertexBuffer::VertexBuffer(void) {}
VertexBuffer::~VertexBuffer()
{
    if(m_Initialized)
        DeleteBuffer();
}

void VertexBuffer::Bind(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void VertexBuffer::Unbind(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::PushLayout(int size)
{
    m_Layout.push_back(size);
}

void VertexBuffer::BufferData(float *data, int count)
{
    // buffer data
    GenerateBuffer();
    Bind();
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), (void *)data, GL_STATIC_DRAW);

    // set layout
    int stride = std::accumulate(m_Layout.begin(), m_Layout.end(), 0);
    int offset = 0;
    for(int i = 0; i < m_Layout.size(); i++) {
        glVertexAttribPointer(i, m_Layout[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);
        offset += m_Layout[i];
    }

    m_Initialized = true;
}

IndexBuffer::IndexBuffer(void) {}
IndexBuffer::~IndexBuffer()
{
    if(m_Initialized)
        DeleteBuffer();
}

void IndexBuffer::Bind(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void IndexBuffer::Unbind(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::BufferData(unsigned int *data, int count)
{
    // buffer data
    GenerateBuffer();
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), (void *)data, GL_STATIC_DRAW);

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

void VertexArray::Bind(void)
{
    glBindVertexArray(m_Id);
}

void VertexArray::Unbind(void)
{
    glBindVertexArray(0);
}

#pragma endregion

#pragma region shader

Shader::Shader(void) {}
Shader::~Shader()
{
    if(m_Initialized)
        glDeleteProgram(m_Id);
}

void Shader::Bind(void)
{
    glUseProgram(m_Id);
}

void Shader::Unbind(void)
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

#pragma endregion