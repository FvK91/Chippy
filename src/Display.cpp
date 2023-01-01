#include "display.h"

#include <iostream>

Display::Display() {
    SDL_Init(SDL_INIT_VIDEO);
    window_ = SDL_CreateWindow("Chippy - a CHIP-8 interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
}

Display::~Display() {
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Display::IsInitialized() const {
    auto report = [](const auto &message, const auto sdl_error) {
        std::cerr << message << "\n SDL Error: " << sdl_error << '\n';
        return false;
    };
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        return report("SDL could not be initialized.", SDL_GetError());
    }
    if (!window_) {
        return report("Window could not be created.", SDL_GetError());
    }
    if (!renderer_) {
        return report("Renderer could not be created", SDL_GetError());
    }
    return true;
}

bool Display::FlipPixel(const uint8_t x, const uint8_t y) {
    screen_[x][y] = !screen_[x][y];
    return screen_[x][y] == 0;
}

void Display::Render() const {
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);
    SDL_SetRenderDrawColor(renderer_, 0x66, 0xFF, 0x66, 0xFF);
    int x = 0;
    for (const auto &pixels_column: screen_) {
        int y = 0;
        for (const auto pixel: pixels_column) {
            if (pixel) {
                SDL_Rect rect = {x, y, PIXEL_WIDTH, PIXEL_HEIGHT};
                SDL_RenderFillRect(renderer_, &rect);
            }
            y += PIXEL_HEIGHT;
        }
        x += PIXEL_WIDTH;
    }

    SDL_RenderPresent(renderer_);
}

void Display::Clear() {
    screen_ = {};
}