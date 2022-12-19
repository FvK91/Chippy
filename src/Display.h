#pragma once

#include <SDL2/SDL.h>

class Display {
public:
    Display();

    ~Display();

private:
    SDL_Window* window_;

    SDL_Renderer* renderer_;

};