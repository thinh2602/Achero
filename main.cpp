#include<SDL.h>
#include<iostream>
#include<algorithm>

#include"src/graphics.h"
#include"src/game.h"
#include"src/config.h"


int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if(!initSDL(&window, &renderer, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }

    SDL_Event event;
    int running = 1;

    while (running) {
        if(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        switch (running) {
            case 0:
                break;
            case 1:
                running = handleGame(&window, &renderer);
                break;
            case 2:
                running = renderEndGameScreen(&renderer);
                break;
        }
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}