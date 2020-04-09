#pragma once

#include <GLFW/glfw3.h>

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
    GLFWwindow *GetGLFWWindow(void);

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
    ~Application();

    void Run(void);

    Window *GetWindow(void);

private:
    Window *m_Window;
    Input m_Input;
};