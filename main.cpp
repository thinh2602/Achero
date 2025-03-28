#include<SDL.h>
#include<iostream>
#include<algorithm>

#include"src/graphics.h"
#include"src/game.h"
#include"src/config.h"


int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if(!initSDL(&window, &renderer, WINDOW_TITLE)) {
        return 1;
    }

    SDL_Event event;
    int running = rendererInitGame(&renderer);
    while (running != -1) {
        switch (running) {
            case -1:
                break;
            case 1:
                running = handleGame(&window, &renderer);
                break;
            default:
                running = renderEndGameScreen(&renderer, running);
                break;
        }
        if(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = -1;
            }
        }
    }


    deInitSDL(&window, &renderer);
    return 0;
}