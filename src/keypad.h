#pragma once

#include <unordered_map>

#include <SDL2/SDL.h>

namespace chip8 {
    class Keypad {
    public:
        void Update(bool &quit);

        [[nodiscard]] bool KeyPressed(int key) const;

        [[nodiscard]] u_int8_t KeyPressed() const;

        [[nodiscard]] bool KeyDown(int key) const;

    private:

        u_int16_t keyboard_state_{}; // Current state

        u_int16_t prev_keyboard_state_{}; // Previous state

        static const std::unordered_map<int, u_int16_t> keyboard_mapping_;
    };
}
