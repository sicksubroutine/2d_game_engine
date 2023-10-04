#include <iostream>
#include "game/game.h"
#include "logger/logger.h"
#include <sol/sol.hpp>


void TestLua() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script_file("./assets/scripts/my_script.lua");

    float my_variable = lua["some_variable"];

    // grab the result of a function call
    sol::function lerp = lua["lerp"];

    // call the function
    float result = lerp(0.2f, 0.8f, 0.5f);

    std::cout << result << std::endl;
    

    std::cout << my_variable << std::endl;
}

int main(int argc, char* argv[]) {

    // Game game;
    // game.Initialize();
    // game.Run();
    // game.Destroy();

    TestLua();

    return 0;
}
