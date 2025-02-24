#include"src/config.h"
#include"src/graphics.h"

#include<iostream>
#include<cmath>
#include<vector>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

const int GRASS_COUNT = 200;
const int MINI_MAP_WIDTH = 150;
const int MINI_MAP_HEIGHT = 150;
const int MINI_MAP_X = WINDOW_WIDTH - MINI_MAP_WIDTH - 10;
const int MINI_MAP_Y = 10;

typedef struct {
    int x, y;
    int bladeCount;
} Grass;

TTF_Font* font = nullptr;

std::vector<Grass> grasses(GRASS_COUNT);

bool IsPointInWindow(int posX, int posY, int width, int height) {
    return (-width <= posX && posX <= WINDOW_WIDTH + width && -height <= posY && posY <= WINDOW_HEIGHT + height);
}

int initSDL(SDL_Window** window, SDL_Renderer** renderer, const char *title, int width, int height) {

    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 0;
    }

    font = TTF_OpenFont("assets/ttf/PressStart2P-Regular.ttf", 24);


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

    centreX += WORLD_ORIGIN_X;
    centreY += WORLD_ORIGIN_Y;

    if (!IsPointInWindow(centreX, centreY, outerRadius, outerRadius)) {
        return;
    }

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

void drawRectangle(SDL_Renderer** renderer, SDL_Rect rect, SDL_Color color, int type, int maxHP, int currentHP) {
    
    if (type) {
        rect.x -= rect.w / 2;
        rect.y -= rect.h / 2;
    }

    rect.x += WORLD_ORIGIN_X;
    rect.y += WORLD_ORIGIN_Y;

    if (!IsPointInWindow(rect.x, rect.y, rect.w, rect.h)) {
        return;
    }

    float percentHP = 1.0f - 1.0f * currentHP / maxHP;

    SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);

    for (int w = 0; w < rect.w; w++) {
        for (int h = 0; h < rect.h; h++) {
            if (w < 4 || h < 4 || rect.w - 4 < w || rect.h - 4 < h || percentHP * (rect.h - 4 + 1) < h) {
                SDL_RenderDrawPoint(*renderer, rect.x + w, rect.y + h);
            }
        }
    }

}

void drawGrass(SDL_Renderer** renderer, int originX, int originY, int type) {

    if (type) {
        for (int i = 0; i < GRASS_COUNT; i++) {
            // Sinh góc và bán kính ngẫu nhiên
            double angle = ((double)rand() / RAND_MAX) * 2 * M_PI; // Góc từ 0 đến 2π
            double radius = ((double)rand() / RAND_MAX) * (WORLD_MAP_WIDTH / 2); // Bán kính từ 0 đến 1000

            // Tính tọa độ theo góc và bán kính
            grasses[i].x = radius * cos(angle) + WORLD_ORIGIN_X;
            grasses[i].y = radius * sin(angle) + WORLD_ORIGIN_Y;
            grasses[i].bladeCount = 3 + rand() % 2; 
        }
    }

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);
    for (auto grass: grasses) {
        grass.x -= originX;
        grass.y -= originY;
        if (!IsPointInWindow(grass.x, grass.y, 50, 50)) {
            continue;
        }
            
        for (int i = 0; i < grass.bladeCount; i++) {
            double angle = (rand() % 50 - 25) * M_PI / 180;  // Góc lệch từ -20 đến 20 độ
            int length = rand() % 20 + 5;  // Chiều dài từ 20 đến 40 px
            int width = 2;  // Độ rộng nhánh cỏ

            int end_x = grass.x + (int)(sin(angle) * length);
            int end_y = grass.y - (int)(cos(angle) * length);

            // Vẽ nhánh cỏ
            SDL_RenderDrawLine(*renderer, grass.x, grass.y, end_x, end_y);
    
            // Đậm hơn một chút bằng cách vẽ thêm các đường gần kề
            SDL_RenderDrawLine(*renderer, grass.x + 1, grass.y, end_x + 1, end_y);
        }
    }
}

void drawScore(SDL_Renderer** renderer, int score) {
    char scoreText[20];

    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText, textColor);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(*renderer, textSurface);


    SDL_Rect textRect;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    textRect.x = (WINDOW_WIDTH - textRect.w) / 2; // Căn giữa theo chiều ngang
    textRect.y = 10;

    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(*renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);

}

void drawMiniMap(SDL_Renderer** renderer, const std::vector<GameObject>& enemies, const GameObject& player) {

    // Tỷ lệ thu nhỏ bản đồ
    const float scale = 1.0f * MINI_MAP_WIDTH / WORLD_MAP_WIDTH;

    // Vẽ nền mini-map
    SDL_Rect miniMapRect = {MINI_MAP_X, MINI_MAP_Y, MINI_MAP_WIDTH, MINI_MAP_HEIGHT};
    SDL_SetRenderDrawColor(*renderer, 50, 50, 50, 200); // Màu xám đậm
    SDL_RenderFillRect(*renderer, &miniMapRect);

    // Viền mini-map
    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255); // Màu trắng
    SDL_RenderDrawRect(*renderer, &miniMapRect);

    // Vẽ player (màu xanh dương)
    SDL_Color playerColor = {0, 0, 255, 255};
    int playerX = MINI_MAP_X + static_cast<int>(player.rect.x * scale) - WORLD_ORIGIN_X + MINI_MAP_WIDTH / 2;
    int playerY = MINI_MAP_Y + static_cast<int>(player.rect.y * scale) - WORLD_ORIGIN_Y + MINI_MAP_HEIGHT / 2;
    drawCircle(renderer, playerX, playerY, 5, playerColor);

    // Vẽ enemies (màu đỏ)
    SDL_Color enemyColor = {255, 0, 0, 255};
    for (const auto& enemy : enemies) {
        int enemyX = MINI_MAP_X + static_cast<int>(enemy.rect.x * scale) - WORLD_ORIGIN_X + MINI_MAP_WIDTH / 2;
        int enemyY = MINI_MAP_Y + static_cast<int>(enemy.rect.y * scale) - WORLD_ORIGIN_Y + MINI_MAP_HEIGHT / 2;
        drawCircle(renderer, enemyX, enemyY, 4, enemyColor);
    }
}

bool renderEndGameScreen(SDL_Renderer** renderer, int score) {

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_RenderClear(*renderer);

    SDL_Color black = {0, 0, 0}; // Định nghĩa màu đen
    
    // Vẽ hộp màu đỏ với chữ "YOU LOSE" và "Score"
    SDL_SetRenderDrawColor(*renderer, 255, 0, 0, 255);
    SDL_Rect messageRect = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 100, 200, 100}; // Tăng chiều cao hộp
    SDL_RenderFillRect(*renderer, &messageRect);

    // Vẽ chữ "YOU LOSE"
    SDL_Surface* messageSurface = TTF_RenderText_Solid(font, "YOU LOSE", black);
    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(*renderer, messageSurface);
    SDL_Rect messageTextRect = {WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 90, 160, 30}; // Căn giữa phía trên hộp
    SDL_RenderCopy(*renderer, messageTexture, NULL, &messageTextRect);
    SDL_FreeSurface(messageSurface);
    SDL_DestroyTexture(messageTexture);

    // -------------------- HIỂN THỊ SCORE BÊN TRONG HỘP --------------------
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score); // Tạo chuỗi score

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, black);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(*renderer, scoreSurface);

    SDL_Rect scoreTextRect = {WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2 - 50, 120, 25}; // Ngay dưới "YOU LOSE"
    SDL_RenderCopy(*renderer, scoreTexture, NULL, &scoreTextRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
    // ----------------------------------------------------------------------

    // Vẽ nút "REPLAY" màu xanh
    SDL_SetRenderDrawColor(*renderer, 0, 255, 0, 255);
    SDL_Rect retryButton = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 20, 90, 50};
    SDL_RenderFillRect(*renderer, &retryButton);

    SDL_Surface* retrySurface = TTF_RenderText_Solid(font, "REPLAY", black);
    SDL_Texture* retryTexture = SDL_CreateTextureFromSurface(*renderer, retrySurface);
    SDL_Rect retryTextRect = {WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 + 30, 70, 30};
    SDL_RenderCopy(*renderer, retryTexture, NULL, &retryTextRect);
    SDL_FreeSurface(retrySurface);
    SDL_DestroyTexture(retryTexture);

    // Vẽ nút "EXIT" màu vàng
    SDL_SetRenderDrawColor(*renderer, 255, 255, 0, 255);
    SDL_Rect exitButton = {WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 + 20, 90, 50};
    SDL_RenderFillRect(*renderer, &exitButton);

    SDL_Surface* exitSurface = TTF_RenderText_Solid(font, "EXIT", black);
    SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(*renderer, exitSurface);
    SDL_Rect exitTextRect = {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2 + 30, 70, 30};
    SDL_RenderCopy(*renderer, exitTexture, NULL, &exitTextRect);
    SDL_FreeSurface(exitSurface);
    SDL_DestroyTexture(exitTexture);

    // Cập nhật renderer để hiển thị tất cả
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
    return ans;
}

void deInitSDL(SDL_Window** window, SDL_Renderer** renderer) {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyWindow(*window);
    SDL_DestroyRenderer(*renderer);
    SDL_Quit();
}
