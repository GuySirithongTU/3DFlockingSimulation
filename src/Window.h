#pragma once

#include <GLFW/glfw3.h>

class Window
{
public:
    static Window *GetInstance(void);
private:
    static Window *s_Instance;

public:
    Window(void);
    Window(int width, int height, const char *title);
    ~Window();

    void SwapBuffers(void);
    void PollEvents(void);
    bool WindowShouldClose(void);

private:
    GLFWwindow *m_Window;
};