#include <iostream>
#include <filesystem>

#include "chip8.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Add path to ROM as input argument. [Optional parameter: IPS]\n";
        return 1;
    } else if (argc > 3) {
        std::cout << "Too many input parameters\n";
        return 1;
    }

    const auto ROM = argv[1];
    if (!std::filesystem::exists(ROM)) {
        std::cerr << "Invalid ROM path\n";
    }

    // Instructions per second
    auto IPS = 1000;
    if (argc == 3) {
        IPS = std::atoi(argv[2]);
    }

    chip8::Config config{false, false};

    chip8::Interpreter chip8_interpreter{config};

    if (!chip8_interpreter.GetDisplay().IsInitialized()) {
        std::cerr << "Display could not be initialized, aborting program.";
        return 1;
    }

    chip8_interpreter.Run(ROM, IPS);

    return 0;
}
