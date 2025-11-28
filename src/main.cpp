// main.cpp
#include "Engine.h"
#include "Game.h"

int main(int argc, char** argv)
{
    Engine engine;
    Game   game;

    engine.setGame(&game);

    if (!engine.init("Bing Chilling C++ Engine", 1024, 768)) {
        return -1;
    }

    engine.run();
    engine.shutdown();

    return 0;
}
