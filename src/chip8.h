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

        bool fx55_incr_I_{ false };
    };

    class Instruction {
    public:
        Instruction(u_int8_t first_byte, u_int8_t second_byte);

        u_int8_t FirstByte() const;

        u_int8_t SecondByte() const;

        u_int8_t Nibble1() const;

        u_int8_t Nibble2() const;

        u_int8_t Nibble3() const;

        u_int8_t Nibble4() const;

        u_int16_t Nibble234() const;

        u_int16_t operator()() const;

    private:
        u_int8_t first_byte_{};

        u_int8_t second_byte_{};
    };

    class Interpreter {
    public:
        static constexpr auto font_address = 0x50;

        Interpreter(Config config) :
        config_(config) {
            std::copy(f.begin(), f.begin() + sizeof(f), RAM_.begin() + font_address);
        }

        int Run(const std::filesystem::path &path, const u_int16_t ips);

        const Display &GetDisp() const;

    private:
        Instruction FetchInstruction() const;

        void ExecuteInstruction(Instruction i);

        int LoadROM(const std::filesystem::path &path);

        std::array<u_int8_t, 4096> RAM_{};
        std::array<u_int8_t, 16> registers_{}; // 0 through F
        u_int16_t I_{}; // I register
        u_int8_t delay_timer_{};
        u_int8_t sound_timer_{};

        int16_t PC_{}; // Program Counter, pointing to current instruction in memory

        // Stack
        stack stack_{};

        // Display
        Display display_{};

        // Config
        Config config_{};

        // Keypad
        Keypad keypad_{};
    };

} // chip8
