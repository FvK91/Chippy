#include <iostream>

#include "chip8.h"
#include "Display.h"

int main(int argc, char *argv[]) {

    chip8::Interpreter chip8_interpreter;

    Display display{};
    if (display.IsInitialized() == 0) {
        Display::Screen screen{};

        for (int y = 0; y < PIXELS_Y; ++y) {
           screen[5][y] = 1;
        }
        for (int x = 0; x < PIXELS_X; ++x) {
            screen[x][5] = 1;
        }

        display.PollAndRender(screen);
    }
    else {
        std::cerr << "Display could not be initialized, aborting program.";
    }

    return 0;
}
