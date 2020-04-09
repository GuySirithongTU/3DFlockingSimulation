#include "Input.h"

#include "Application.h"

#include <iostream>
#include <cstring>

Input *Input::s_Instance = nullptr;
Input *Input::GetInstance(void)
{
    return s_Instance;
}

Input::Input(void)
{
    if(s_Instance == nullptr) {
        s_Instance = this;
    } else {
        std::cout << "INPUT::ERROR: creating second input manager" << std::endl;
        return;
    }

    std::memset(m_KeyStates, 0, KEY_COUNT * sizeof(InputState));
    std::memset(m_MouseButtonStates, 0, MOUSE_BUTTON_COUNT * sizeof(InputState));
}

Input::~Input()
{

}

bool Input::GetKeyDown(int keyCode) const   { return m_KeyStates[keyCode].down; }
bool Input::GetKey(int keyCode) const       { return glfwGetKey(Application::GetInstance()->GetWindow()->GetGLFWWindow(), keyCode); }
bool Input::GetKeyUp(int keyCode) const     { return m_KeyStates[keyCode].up; }
bool Input::GetMouseButtonDown(int mouseButtonCode) const   { return m_MouseButtonStates[mouseButtonCode].down; }
bool Input::GetMouseButton(int mouseButtonCode) const       { glfwGetMouseButton(Application::GetInstance()->GetWindow()->GetGLFWWindow(), mouseButtonCode); }
bool Input::GetMouseButtonUp(int mouseButtonCode) const     { return m_MouseButtonStates[mouseButtonCode].up; }

std::pair<int, int> Input::GetMousePosition(void) const
{
    double x, y;
    glfwGetCursorPos(Application::GetInstance()->GetWindow()->GetGLFWWindow(), &x, &y);
    return std::make_pair(x, y);
}

void Input::InitCallbacks(void)
{
    GLFWwindow *window = Application::GetInstance()->GetWindow()->GetGLFWWindow();

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
}

void Input::PollEvents(void)
{
    for(int i = 0; i < KEY_COUNT; i++) {
        m_KeyStates[i].up = false;
        m_KeyStates[i].down = false;
    }

    glfwPollEvents();
}

void Input::KeyCallback(GLFWwindow *window, int keyCode, int scanCode, int action, int mods)
{
    switch(action) {
        case GLFW_PRESS:
            Input::GetInstance()->m_KeyStates[keyCode].down = true;
            break;
        case GLFW_RELEASE:
            Input::GetInstance()->m_KeyStates[keyCode].up = true;
            break;
    }
}

void Input::MouseButtonCallback(GLFWwindow *window, int mouseButtonCode, int action, int mods)
{
    switch(action) {
        case GLFW_PRESS:
            Input::GetInstance()->m_MouseButtonStates[mouseButtonCode].down = true;
            break;
        case GLFW_RELEASE:
            Input::GetInstance()->m_KeyStates[mouseButtonCode].up = true;
            break;
    }
}