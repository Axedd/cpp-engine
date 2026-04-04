// main.cpp
#include "Engine.h"
#include "Game.h"
#include "../../EngineShared/AssetFormat.h"
#include "../include/Engine/Resources/ResourceManager.h"

int main(int argc, char** argv)
{
    Engine engine;
    Game   game;

    engine.setGame(&game);

    if (!engine.init("Bing Chilling C++ Engine", 1024, 768)) {
        return -1;
    }

    //LoadAsset("test.asset");

    engine.run();
    engine.shutdown();

    return 0;
}
