#pragma once

#include <array>

// ToDo, stack overflow error

namespace chip8 {

    class stack {
    public:
        void Push(const u_int16_t addr);

        u_int16_t Pop();
    private:
        u_int8_t SP_{}; // Stack pointer, pointing to top-level of stack

        std::array<u_int16_t, 16> stack_{};
    };

} // chip8