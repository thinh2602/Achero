#include"src/config.h"
#include"src/graphics.h"

#include<iostream>
#include<cmath>
#include<vector>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

typedef struct {
    int x, y;
    int bladeCount;
} Grass;

std::vector<Grass> grass;

int initSDL(SDL_Window** window, SDL_Renderer** renderer, const char *title, int width, int height) {

    srand(static_cast<unsigned int>(time(nullptr)));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << '\n'; 
        return 0;
    }

    *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

    if (!*window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    if (!*renderer) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
    }
    return 1;

}

void drawCircle(SDL_Renderer** renderer, int centreX, int centreY, int outerRadius, SDL_Color color, const int maxHP, int currentHP) {

    float percentHP = 1.0f - 1.0f * currentHP / maxHP;

    int innerRadius = outerRadius - 3;

    SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < outerRadius * 2; w ++) {
        for (int h = 0; h < outerRadius * 2; h ++) {
            int dx = outerRadius - w;
            int dy = outerRadius - h;
            int r = dx * dx + dy * dy;
            if (r <= outerRadius * outerRadius && (innerRadius * innerRadius <= r || 2.0f * innerRadius * percentHP <= 2 * (innerRadius + 1) - h)) {
                SDL_RenderDrawPoint(*renderer, centreX + dx, centreY + dy);
            }
        }
    }
}

void drawRectangle(SDL_Renderer** renderer, SDL_Rect rect, SDL_Color color, int hp) {

    SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);

    for (int w = 0; w < rect.w; w++) {
        for (int h = 0; h < rect.h; h++) {
            SDL_RenderDrawPoint(*renderer, rect.x + w, rect.y + h);
        }
    }

}


void drawGrass(SDL_Renderer** renderer) {
}

bool renderEndGameScreen(SDL_Renderer** renderer) {

    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return -1;
    }

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_RenderClear(*renderer);

    TTF_Font* font = TTF_OpenFont("assets/ttf/PressStart2P-Regular.ttf", 24);

    SDL_Color white = {0, 0, 0}; // Định nghĩa màu trắng
    
    // Vẽ hộp màu đỏ với chữ "YOU LOSE"
    SDL_SetRenderDrawColor(*renderer, 255, 0, 0, 255);
    SDL_Rect messageRect = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 75, 200, 50};
    SDL_RenderFillRect(*renderer, &messageRect);

    SDL_Surface* messageSurface = TTF_RenderText_Solid(font, "YOU LOSE", white);
    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(*renderer, messageSurface);
    SDL_Rect messageTextRect = {WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 65, 160, 30};
    SDL_RenderCopy(*renderer, messageTexture, NULL, &messageTextRect);
    SDL_FreeSurface(messageSurface);
    SDL_DestroyTexture(messageTexture);

    // Vẽ nút "REPLAY" màu xanh
    SDL_SetRenderDrawColor(*renderer, 0, 255, 0, 255);
    SDL_Rect retryButton = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2, 90, 50};
    SDL_RenderFillRect(*renderer, &retryButton);

    SDL_Surface* retrySurface = TTF_RenderText_Solid(font, "REPLAY", white);
    SDL_Texture* retryTexture = SDL_CreateTextureFromSurface(*renderer, retrySurface);
    SDL_Rect retryTextRect = {WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 + 10, 70, 30};
    SDL_RenderCopy(*renderer, retryTexture, NULL, &retryTextRect);
    SDL_FreeSurface(retrySurface);
    SDL_DestroyTexture(retryTexture);

    // Vẽ nút "EXIT" màu vàng
    SDL_SetRenderDrawColor(*renderer, 255, 255, 0, 255);
    SDL_Rect exitButton = {WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2, 90, 50};
    SDL_RenderFillRect(*renderer, &exitButton);

    SDL_Surface* exitSurface = TTF_RenderText_Solid(font, "EXIT", white);
    SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(*renderer, exitSurface);
    SDL_Rect exitTextRect = {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2 + 10, 70, 30};
    SDL_RenderCopy(*renderer, exitTexture, NULL, &exitTextRect);
    SDL_FreeSurface(exitSurface);
    SDL_DestroyTexture(exitTexture);


        
    // Cập nhật renderer để hiển thị các nút
    SDL_RenderPresent(*renderer);
        
    // Xử lý sự kiện chuột khi nhấn nút
    SDL_Event event;
    bool waitingForInput = true;
    bool ans;
    
    while (waitingForInput) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waitingForInput = false;
                ans = false;
                break;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
        
                // Kiểm tra nếu bấm vào nút "Chơi lại"
                if (mouseX >= retryButton.x && mouseX <= retryButton.x + retryButton.w &&
                    mouseY >= retryButton.y && mouseY <= retryButton.y + retryButton.h) {

                    ans = true;
                    waitingForInput = false;
                    break;
                }
        
                // Kiểm tra nếu bấm vào nút "Thoát game"
                if (mouseX >= exitButton.x && mouseX <= exitButton.x + exitButton.w &&
                    mouseY >= exitButton.y && mouseY <= exitButton.y + exitButton.h) {

                                
                    waitingForInput = false;
                    ans = false;
                    break;
                }
            }
        }        
    }
    TTF_CloseFont(font);
    TTF_Quit();
    return ans;
}
