#include <iostream>
#include <filesystem>

#include "chip8.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Add path to ROM as input argument\n";
        return 1;
    } else if (argc > 2) {
        std::cout << "Too many input parameters\n";
        return 1;
    }

    const auto ROM = argv[1];
    if (!std::filesystem::exists(ROM)) {
        std::cerr << "Invalid ROM path\n";
    }

    chip8::Config config{false, true};

    chip8::Interpreter chip8_interpreter{config};

    if (!chip8_interpreter.GetDisplay().IsInitialized()) {
        std::cerr << "Display could not be initialized, aborting program.";
        return 1;
    }

    chip8_interpreter.Run(ROM, 700);

    return 0;
}
