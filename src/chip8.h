#pragma once

#include "display.h"
#include "keypad.h"
#include "stack.h"

#include <array>
#include <filesystem>

using font = std::array<u_int8_t, 80>;
constexpr font f = {0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                    0x20, 0x60, 0x20, 0x20, 0x70, // 1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

namespace chip8 {
    struct Config {
        bool shift_set_VY_{};

        bool fx55_incr_I_{};
    };

    class Instruction {
    public:
        Instruction(u_int8_t first_byte, u_int8_t second_byte);

        [[nodiscard]] u_int8_t B2() const; // Byte 2

        [[nodiscard]] u_int8_t N1() const; // Nibble 1

        [[nodiscard]] u_int8_t N2() const; // Nibble 2

        [[nodiscard]] u_int8_t N3() const; // Nibble 3

        [[nodiscard]] u_int8_t N4() const; // Nibble 4

        [[nodiscard]] u_int16_t N234() const; // Nibbles 234

        u_int16_t operator()() const;

    private:
        u_int8_t byte1_{};

        u_int8_t byte2_{};
    };

    class Interpreter {
    public:
        static constexpr auto font_address = 0x50;

        explicit Interpreter(Config config) : config_(config) {
            std::copy(f.begin(), f.begin() + sizeof(f), RAM_.begin() + font_address);
        }

        int Run(const std::filesystem::path &path, u_int16_t ips);

        [[nodiscard]] const Display &GetDisplay() const;

    private:
        [[nodiscard]] Instruction FetchInstruction() const;

        void ExecuteInstruction(Instruction i);

        int LoadROM(const std::filesystem::path &path);

        std::array<u_int8_t, 4096> RAM_{};
        std::array<u_int8_t, 16> V_{}; // Registers 0..F
        u_int16_t I_{}; // I register
        u_int8_t delay_timer_{};
        u_int8_t sound_timer_{};
        u_int16_t PC_{}; // Program Counter, pointing to current instruction in memory
        stack stack_{};
        Display display_{};
        Config config_{};
        Keypad keypad_{};
    };

} // chip8
