/*#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
int main(){
    int buffer[16][16];
    for(int i = 0; i<16; i++){
        for(int j = 0; j<16; j++){
            if ((i + j) % 2 == 0) {
    buffer[i][j] = 1;
} else {
    buffer[i][j] = 0;
}
        }
    }
    const int SCALE = 20;
    SDL_Window *window = SDL_CreateWindow("play",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,640,320,0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for(int i = 0; i<16; i++){
            for (int j = 0; j<16; j++){
                if(buffer[i][j]){
                    SDL_Rect rect{i*SCALE,j*SCALE,SCALE,SCALE};
                    SDL_RenderFillRect(renderer,&rect);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}*/