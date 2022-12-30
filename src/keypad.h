#pragma once

#include <map>

#include <SDL2/SDL.h>

namespace chip8 {
    class Keypad {
    public:
        void Update();

        bool KeyPressed(int key) const;

        u_int8_t KeyPressed() const;

        bool KeyDown(int key) const;

    private:

        u_int16_t keyboard_state_{}; // Current state

        u_int16_t prev_keyboard_state_{}; // Previous state

        static const std::map<int, u_int16_t> keyboard_mapping_;
    };
}
