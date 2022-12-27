#include <iostream>

#include "chip8.h"

int main(int argc, char *argv[]) {

    chip8::Interpreter chip8_interpreter;

    if (!chip8_interpreter.GetDisp().IsInitialized()) {
        std::cerr << "Display could not be initialized, aborting program.";
        return 1;
    }

    chip8_interpreter.Run("../dat/test_opcode.ch8", 1500);

    return 0;
}
