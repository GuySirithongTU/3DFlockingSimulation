#include "Simulation.h"

#include <cstdlib>
#include <ctime>

int main(void)
{
    srand(time(NULL));

    Simulation app;
    app.Run();

    return 0;
}