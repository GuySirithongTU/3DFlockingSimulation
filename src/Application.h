#pragma once

#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Input.h"

class Window
{
public:
    Window(void);
    Window(int width, int height, const char *title);
    ~Window();

    void SwapBuffers(void);
    bool WindowShouldClose(void);

    int GetWidth(void) const;
    int GetHeight(void) const;
    float GetAspect(void) const;
    GLFWwindow *GetGLFWWindow(void);

private:
    static void ResizeCallback(GLFWwindow *window, int width, int height);

private:
    GLFWwindow *m_Window;
    int m_Width, m_Height;
};

class Application
{
public:
    static Application *GetInstance(void);
private:
    static Application *s_Instance;

public:
    Application(void);
    virtual ~Application();

    void Run(void);

    Window *GetWindow(void);

protected:
    virtual void OnInit(void);
    virtual void OnUpdate(void);

protected:
    Window *m_Window;
    Renderer m_Renderer;
    Input m_Input;
};