#pragma region

#include <vector>

#pragma region primitive

class Primitive
{
public:
    Primitive(void);
    virtual ~Primitive();

    virtual void Bind(void) = 0;
    virtual void Unbind(void) = 0;

protected:
    void GenerateBuffer(void);
    void DeleteBuffer(void);

protected:
    unsigned int m_Id = -1;
    bool m_Initialized = false;
};

#pragma endregion

#pragma region buffers

class VertexBuffer : public Primitive
{
public:
    VertexBuffer(void);
    virtual ~VertexBuffer();

    virtual void Bind(void) override;
    virtual void Unbind(void) override;

    void PushLayout(int size);
    void BufferData(float *data, int count);

private:
    std::vector<int> m_Layout;
};

class IndexBuffer : public Primitive
{
public:
    IndexBuffer(void);
    virtual ~IndexBuffer();

    virtual void Bind(void) override;
    virtual void Unbind(void) override;

    void BufferData(unsigned int *data, int count);
};

#pragma endregion

#pragma region vertex_array

class VertexArray : public Primitive
{
public:
    VertexArray(void);
    ~VertexArray();

    virtual void Bind(void) override;
    virtual void Unbind(void) override;

    void Init(void);
};

#pragma endregion

#pragma region shader

class Shader : public Primitive
{
public:
    Shader(void);
    ~Shader();

    virtual void Bind(void) override;
    virtual void Unbind(void) override;

    void InitShader(const char *vertexPath, const char *fragmentPath);

private:
    unsigned int CompileShader(const char *path, unsigned int type);
};

#pragma endregion