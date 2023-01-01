#include <iostream>

#include "chip8.h"

int main(int argc, char *argv[]) {
    chip8::Config config{ false, true };

    chip8::Interpreter chip8_interpreter{config};

    if (!chip8_interpreter.GetDisp().IsInitialized()) {
        std::cerr << "Display could not be initialized, aborting program.";
        return 1;
    }

    // const auto ROM = "../dat/IBM_Logo.ch8";
    // const auto ROM = "../dat/test_opcode.ch8";
    // const auto ROM = "../dat/bc_test.ch8";
    // const auto ROM = "../dat/Puzzle.ch8";
    // const auto ROM = "../dat/Airplane.ch8";
    const auto ROM = "../dat/15puzzle.ch8";
    chip8_interpreter.Run(ROM, 1000);

    return 0;
}
