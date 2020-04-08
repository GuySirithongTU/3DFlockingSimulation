#include "Window.h"

int main(void)
{
    Window window(800, 600, "Flocking Simulation");

    while(!window.WindowShouldClose())
    {
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}