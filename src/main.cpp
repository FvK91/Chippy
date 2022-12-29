#include <iostream>

#include "chip8.h"

int main(int argc, char *argv[]) {

    chip8::Interpreter chip8_interpreter{{}};

    if (!chip8_interpreter.GetDisp().IsInitialized()) {
        std::cerr << "Display could not be initialized, aborting program.";
        return 1;
    }

    const auto ROM = "../dat/IBM_Logo.ch8";
    // const auto ROM = "../dat/test_opcode.ch8";
    // const auto ROM = "../dat/bc_test.ch8";
    chip8_interpreter.Run(ROM, 10000);

    return 0;
}
