#include <iostream>
#include "memory/memory.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window =
        SDL_CreateWindow(
            "Test",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            640,
            320,
            0);

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED);

    bool running = true;
    const int SCALE = 10;
    Chip8 chip8;
    chip8.init();
    chip8.loadROM(R"(C:\Users\sanmi\Downloads\IBM Logo.ch8)");
    while(running)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        chip8.Cycle();
        for(int y = 0; y < 32; y++)
        {
            for(int x = 0; x < 64; x++)
            {
                if(chip8.display[y * 64 + x])
                {
                    SDL_Rect rect{
                        x * SCALE,
                        y * SCALE,
                        SCALE,
                        SCALE
                    };

                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}