#include "Display.h"

#include <iostream>

constexpr auto SCREEN_WIDTH = 1280;
constexpr auto SCREEN_HEIGHT = 640;
constexpr auto PIXEL_WIDTH = SCREEN_HEIGHT / 64;
constexpr auto PIXEL_HEIGHT = SCREEN_HEIGHT / 32;

Display::Display() {

    auto report = [](const auto& message, const auto sdl_error){
        std::cerr << message << '\n';
        std::cerr << "SDL Error: " << sdl_error << '\n';
    };

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        report("SDL could not be initialized.", SDL_GetError());
    }
    else {
        window_ = SDL_CreateWindow(
                "Chippy - a CHIP-8 interpreter",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
        );
        if (!window_) {
            report("Window could not be created.", SDL_GetError());
        }
        else {
            renderer_ = SDL_CreateRenderer( window_, -1, SDL_RENDERER_ACCELERATED);
            if (!renderer_){
                report("Renderer could not be created", SDL_GetError());
            }

            //Event handler
            SDL_Event e;
            bool quit{};
            while(!quit) {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    if (e.type == SDL_QUIT){
                        quit = true;
                    }
                }

                SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer_);

                // Rect
                SDL_Rect rect = { 0, 0, PIXEL_WIDTH, PIXEL_HEIGHT };
                SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderFillRect(renderer_, &rect);

                SDL_RenderPresent(renderer_);

                SDL_Delay(5000);
            }
        }
    }
}

Display::~Display() {
    SDL_DestroyWindow(window_);

    SDL_Quit();
}
