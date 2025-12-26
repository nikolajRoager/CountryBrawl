#include <iostream>

#include "engine.h"

int main() {
    std::cout << "Hello, World! Let us play a game" << std::endl;
    try {
        engine game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr<<"The game crashed due to an unhandled exception "<<e.what()<<std::endl;
        return 1;
    }
    return 0;
}
