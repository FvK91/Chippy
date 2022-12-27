#include <iostream>

#include "chip8.h"

int main(int argc, char *argv[]) {

    chip8::Interpreter chip8_interpreter;

    if (!chip8_interpreter.GetDisp().IsInitialized()) {
        std::cerr << "Display could not be initialized, aborting program.";
        return 1;
    }

    chip8_interpreter.Run("../dat/IBM_Logo.ch8", 100);

    return 0;

    Display display{};
    if (display.IsInitialized() == 0) {
        Display::Screen screen{};

        for (int y = 0; y < PIXELS_Y; ++y) {
            screen[5][y] = 1;
        }
        for (int x = 0; x < PIXELS_X; ++x) {
            screen[x][5] = 1;
        }

        // display.PollAndRender();
    } else {

    }

    return 0;
}
