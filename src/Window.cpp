#include "Window.h"

#include <iostream>

Window *Window::s_Instance = nullptr;
Window *Window::GetInstance(void)
{
    return s_Instance;
}

Window::Window(void)
    : m_Window(nullptr) {}

Window::Window(int width, int height, const char *title)
{
    // initialize singleton instance
    if(s_Instance == nullptr) {
        s_Instance = this;
    } else {
        std::cout << "WINDOW::ERROR: creating second window" << std::endl;
        return;
    }


    // initialize glfw
    if(!glfwInit()) {
        std::cout << "WINDOW::ERROR: cannot initialize glfw" << std::endl;
        return;
    }

    // create window
    m_Window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!m_Window) {
        std::cout << "WINDOW::ERROR: cannot create window" << std::endl;
        glfwTerminate();
        return;
    }

    // make window context current
    glfwMakeContextCurrent(m_Window);
}

Window::~Window()
{
    // terminate glfw
    glfwTerminate();

    s_Instance = nullptr;
}

void Window::SwapBuffers(void)
{
    glfwSwapBuffers(m_Window);
}

void Window::PollEvents(void)
{
    glfwPollEvents();
}

bool Window::WindowShouldClose(void)
{
    return glfwWindowShouldClose(m_Window);
}