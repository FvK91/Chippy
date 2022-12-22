#pragma once

#include <SDL2/SDL.h>

#include <array>

constexpr auto SCREEN_WIDTH = 1280;
constexpr auto SCREEN_HEIGHT = 640;
constexpr auto PIXELS_X = 64;
constexpr auto PIXELS_Y = 32;
constexpr auto PIXEL_WIDTH = SCREEN_WIDTH / PIXELS_X;
constexpr auto PIXEL_HEIGHT = SCREEN_HEIGHT / PIXELS_Y;

class Display {
public:
    using Screen = std::array<std::array<uint8_t, PIXELS_Y>, PIXELS_X>;

    Display();

    ~Display();

    int IsInitialized() const;

    void PollAndRender() const;

    void Clear();

private:
    void Render() const;

    SDL_Window *window_;

    SDL_Renderer *renderer_;

    Screen screen_;
};