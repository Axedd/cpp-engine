#include <SDL.h>
#include <iostream>
#include "Engine.h"

int main(int argc, char** argv)
{
    Engine engine;

    if (!engine.init("My C++ Engine", 1280, 720)) {
        std::cout << "Engine failed to initialize\n";
        return -1;
    }

    engine.run();

    return 0;
}