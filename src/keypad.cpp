#include "keypad.h"

const std::map<int, u_int16_t> chip8::Keypad::keyboard_mapping_ = {{SDL_SCANCODE_X, 0},
                                                                   {SDL_SCANCODE_1, 1},
                                                                   {SDL_SCANCODE_2, 2},
                                                                   {SDL_SCANCODE_3, 3},
                                                                   {SDL_SCANCODE_Q, 4},
                                                                   {SDL_SCANCODE_W, 5},
                                                                   {SDL_SCANCODE_E, 6},
                                                                   {SDL_SCANCODE_A, 7},
                                                                   {SDL_SCANCODE_S, 8},
                                                                   {SDL_SCANCODE_D, 9},
                                                                   {SDL_SCANCODE_Z, 10},
                                                                   {SDL_SCANCODE_C, 11},
                                                                   {SDL_SCANCODE_4, 12},
                                                                   {SDL_SCANCODE_R, 13},
                                                                   {SDL_SCANCODE_F, 14},
                                                                   {SDL_SCANCODE_V, 15},};

void chip8::Keypad::Update() {
    // Save previous keyboard state
    prev_keyboard_state_ = keyboard_state_;

    SDL_Event event;
    bool quit = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
    }

    // Reset current state
    keyboard_state_ = {};

    const auto keyboard_state = SDL_GetKeyboardState(NULL);
    for (const auto [scancode, index]: keyboard_mapping_) {
        if (keyboard_state[scancode]) {
            keyboard_state_ |= 1 << index;
        }
    }
}

bool chip8::Keypad::KeyPressed(int key) const {
    const auto k = 1 << key;
    return (k & keyboard_state_) && !(k & prev_keyboard_state_);
}

u_int8_t chip8::Keypad::KeyPressed() const {
    for (auto k = 0; k < sizeof(keyboard_state_); ++k) {
        if (KeyPressed(k)) {
            return k;
        }
    }
    return 0;
}

bool chip8::Keypad::KeyDown(int key) const {
    return 1 << key & keyboard_state_;
}

