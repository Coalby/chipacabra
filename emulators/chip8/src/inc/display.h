#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <array>
#include <iostream>

#define SDL_ERROR_COUT(message) std::cout << message << " Error: " << SDL_GetError() << std::endl

namespace pixels {
    constexpr int DISPLAY_WIDTH = 64;
    constexpr int DISPLAY_HEIGHT = 32;
    constexpr int WHITE_PIXEL = 0xFFFFFFFF;
    constexpr int BLACK_PIXEL = 0xFF000000;

    using Pixel = uint32_t;
    using PixelRow = std::array<Pixel, DISPLAY_WIDTH>;
    using PixelBuffer = std::array<PixelRow, DISPLAY_HEIGHT>;
}

// TODO: Find a way to make this a singleton
class Display {
    public:
        Display() {
            int error;
            
            // TODO: Handle errors
            error = SDL_Init(SDL_INIT_VIDEO);
            if(error < 0) {
                SDL_ERROR_COUT("SDL_Init has failed!");
            }

            emulatorWindow = SDL_CreateWindow("Chipacabra", SDL_WINDOWPOS_UNDEFINED, 
                                                SDL_WINDOWPOS_UNDEFINED, pixels::DISPLAY_WIDTH, 
                                                pixels::DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);
            if(emulatorWindow == NULL) {
                SDL_ERROR_COUT("Window could not be created!");
            }

            emulatorRenderer = SDL_CreateRenderer(emulatorWindow, -1, SDL_RENDERER_ACCELERATED);
            if(emulatorRenderer == NULL) {
                SDL_ERROR_COUT("Renderer could not be created!");
            }

            emulatorTexture = SDL_CreateTexture(emulatorRenderer, SDL_PIXELFORMAT_ABGR8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                pixels::DISPLAY_WIDTH, pixels::DISPLAY_HEIGHT);
            if(emulatorTexture == NULL) {
                SDL_ERROR_COUT("Texture could not be created!");
            }
        };
        ~Display() {
            SDL_DestroyTexture(emulatorTexture);
            emulatorTexture = NULL;

            SDL_DestroyRenderer(emulatorRenderer);
            emulatorRenderer = NULL;

            SDL_DestroyWindow(emulatorWindow);
            emulatorWindow = NULL;

            SDL_Quit();
        };

        void renderDisplay(const pixels::PixelBuffer& pixels) {
            SDL_UpdateTexture(emulatorTexture, NULL, &pixels, pixels::DISPLAY_WIDTH * sizeof(uint32_t));
            
            if (SDL_RenderClear(emulatorRenderer) != 0)
            {
                SDL_ERROR_COUT("SDL_RenderClear failed!");
                return;
            }

            if (SDL_RenderCopy(emulatorRenderer, emulatorTexture, NULL, NULL) != 0)
            {
                SDL_ERROR_COUT("SDL_RenderCopy failed!");
                return;
            }

            SDL_RenderPresent(emulatorRenderer);

            return;
        }

        char closeDisplayCheck() {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    return 0;   // Close Window
                }
            }

            return 1;   // Keep Window open
        };

    private:
        SDL_Window* emulatorWindow {};
        SDL_Renderer* emulatorRenderer {};
        SDL_Texture* emulatorTexture {};
        SDL_Event event {};
};

#endif
