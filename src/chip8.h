#pragma once

#include <array>

using font = std::array<u_int16_t, 80>;
constexpr font f = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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

    class Interpreter {
    public:
        Interpreter(){
            std::copy(f.begin(),f.begin() + sizeof(f),RAM_.begin() + 0x50);
        }
    private:
        std::array<u_int8_t, 4096> RAM_{};
        std::array<u_int8_t, 16> registers_; // 0 through F
        u_int16_t I_; // I register
        u_int8_t VF_; // Flag register, used as a flag by some instructions
        u_int8_t delay_timer_;
        u_int8_t sound_timer_;

        u_int16_t PC_; // Program Counter, pointing to current instruction in memory

        // Stack
        u_int8_t SP_; // Stack pointer, pointing to top-level of stack
        std::array<u_int16_t, 16> stack_;

        // Display
    };

} // chip8