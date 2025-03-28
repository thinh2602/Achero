#include"src/config.h"
#include"src/graphics.h"

#include<iostream>
#include<cmath>
#include<vector>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

const int GRASS_COUNT = 200;
const int MINI_MAP_WIDTH = 128;
const int MINI_MAP_HEIGHT = 128;
const int MINI_MAP_X = WINDOW_WIDTH - MINI_MAP_WIDTH - 15;
const int MINI_MAP_Y = 15;

typedef struct {
    int x, y;
    int bladeCount;
} Grass;



std::vector<TTF_Font*> fonts(50, nullptr);
std::vector<Grass> grasses(GRASS_COUNT);
std::vector<SDL_Point> fences;

bool IsPointInWindow(int posX, int posY, int width, int height) {
    return (-width <= posX && posX <= WINDOW_WIDTH + width && -height <= posY && posY <= WINDOW_HEIGHT + height);
}

int initSDL(SDL_Window** window, SDL_Renderer** renderer, const char *title, int width, int height) {

    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 0;
    }

    for (int size = 1; size <= 50; size++) {
        fonts[size] = TTF_OpenFont("assets/ttf/PressStart2P-Regular.ttf", size);
    }

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
    // Tính offset theo world
    centreX += WORLD_ORIGIN_X;
    centreY += WORLD_ORIGIN_Y;

    // Nếu vòng tròn không nằm trong cửa sổ, thoát hàm
    if (!IsPointInWindow(centreX, centreY, outerRadius, outerRadius)) {
        return;
    }

    // Tỷ lệ HP (1.0: hết máu, 0.0: đầy máu)
    float percentHP = 1.0f - (float)currentHP / maxHP;
    // Độ dày viền được giả định là 3 pixel
    int innerRadius = outerRadius - 3;
    
    // Tính trước các giá trị bình phương
    int outerRadiusSq = outerRadius * outerRadius;
    int innerRadiusSq = innerRadius * innerRadius;

    /* 
       Ban đầu, trong phiên bản gốc, điều kiện vẽ điểm là:
         if (r <= outerRadius^2 && (r >= innerRadius^2 || 2.0f * innerRadius * percentHP <= 2*(innerRadius+1) - h))
       Trong đó:
         - Với hệ tọa độ ban đầu: h chạy từ 0 đến 2*outerRadius, và dy = outerRadius - h.
         => h = outerRadius - dy.
       Ta có điều kiện tương đương:
         2.0f * innerRadius * percentHP <= 2*(innerRadius+1) - (outerRadius - dy)
       => dy >= 2.0f * innerRadius * percentHP - 2*(innerRadius+1) + outerRadius.
       Ta tính ngưỡng này một lần:
    */
    float hpDyThresholdFloat = 2.0f * innerRadius * percentHP - 2.0f * (innerRadius + 1) + outerRadius;
    // Lấy làm số nguyên (có thể làm tròn lên)
    int hp_dy_threshold = (int)ceil(hpDyThresholdFloat);

    // Cài đặt màu vẽ
    SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);

    // Duyệt theo từng hàng theo dy (từ -outerRadius đến outerRadius)
    for (int dy = -outerRadius; dy <= outerRadius; dy++) {
        int y_coord = centreY + dy;
        // Tính khoảng cách x tối đa ứng với dòng dy (điểm phải nằm trong vòng tròn)
        int dx_max = (int)floor(sqrt((double)outerRadiusSq - dy * dy));
        
        // Nếu dòng này nằm trong vùng hiển thị HP (theo điều kiện chuyển đổi từ h)
        // Với h = outerRadius - dy, điều kiện trở thành: dy >= hp_dy_threshold.
        if (dy >= hp_dy_threshold) {
            // Vẽ toàn bộ đoạn ngang nằm trong vòng tròn
            SDL_RenderDrawLine(*renderer, centreX - dx_max, y_coord, centreX + dx_max, y_coord);
        } else {
            // Với các dòng không thỏa ngưỡng HP, chỉ vẽ các điểm nằm ngoài vòng tròn trong (vùng viền)
            // Ta cần tìm giá trị dx_min sao cho với |dx| >= dx_min thì:
            //    dx^2 + dy^2 >= innerRadiusSq.
            int dx_min = 0;
            if (innerRadiusSq > dy * dy) {
                dx_min = (int)ceil(sqrt((double)innerRadiusSq - dy * dy));
            }
            // Vẽ đoạn bên trái (nếu có)
            if (dx_min <= dx_max) {
                // Vẽ từ x = centreX - dx_max đến x = centreX - dx_min
                SDL_RenderDrawLine(*renderer, centreX - dx_max, y_coord, centreX - dx_min, y_coord);
                // Vẽ đoạn bên phải: từ x = centreX + dx_min đến x = centreX + dx_max
                SDL_RenderDrawLine(*renderer, centreX + dx_min, y_coord, centreX + dx_max, y_coord);
            }
        }
    }
}

void drawRectangle(SDL_Renderer** renderer, SDL_Rect rect, SDL_Color color, int type, int maxHP, int currentHP) {
    // Nếu type != 0, căn chỉnh rect sao cho tâm của hình là vị trí đã cho
    if (type) {
        rect.x -= rect.w / 2;
        rect.y -= rect.h / 2;
    }

    // Cộng thêm offset của world
    rect.x += WORLD_ORIGIN_X;
    rect.y += WORLD_ORIGIN_Y;

    // Nếu hình không nằm trong cửa sổ, không vẽ
    if (!IsPointInWindow(rect.x, rect.y, rect.w, rect.h)) {
        return;
    }

    // Tính tỷ lệ HP (0: đầy HP, 1: hết HP)
    float percentHP = 1.0f - (float)currentHP / maxHP;

    // Cài đặt màu vẽ
    SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);

    const int border = 4;       // Độ dày viền
    const int W = rect.w;       // Chiều rộng của hình
    const int H = rect.h;       // Chiều cao của hình
    // Giá trị ngưỡng HP: nếu h > T thì pixel đó được vẽ (điều chỉnh vùng thể hiện HP)
    const int T = (int)(percentHP * (H - 3));  // Vì công thức gốc: percentHP * (rect.h - 4 + 1)

    // Duyệt qua từng dòng (h)
    for (int h = 0; h < H; h++) {
        int currentY = rect.y + h;

        // Nếu dòng thuộc vùng biên trên, biên dưới hoặc vùng HP thì vẽ cả dòng
        if (h < border || h > H - border - 1 || h > T) {
            SDL_RenderDrawLine(*renderer, rect.x, currentY, rect.x + W - 1, currentY);
        } else {
            // Ở các dòng nội bộ: chỉ vẽ viền trái và phải
            // Vẽ viền trái
            SDL_RenderDrawLine(*renderer, rect.x, currentY, rect.x + border - 1, currentY);
            // Vẽ viền phải
            SDL_RenderDrawLine(*renderer, rect.x + W - border, currentY, rect.x + W - 1, currentY);
        }
    }
}

void drawRegularPolygon(SDL_Renderer** renderer, SDL_Rect rect, SDL_Color color, int sides, int type, int maxHP, int currentHP) {
    // Nếu type khác 0, cho rằng rect.x, rect.y là tọa độ tâm, điều chỉnh về góc trên-trái
    if (type) {
        rect.x -= rect.w / 2;
        rect.y -= rect.h / 2;
    }

    // Cộng thêm offset của world
    rect.x += WORLD_ORIGIN_X;
    rect.y += WORLD_ORIGIN_Y;

    // Nếu hộp chứa không nằm trong cửa sổ, không vẽ
    if (!IsPointInWindow(rect.x, rect.y, rect.w, rect.h)) {
        return;
    }

    // Tính tỷ lệ HP (0: đầy HP, 1: hết HP)
    float percentHP = 1.0f - ((float)currentHP / maxHP);

    // Cài đặt màu vẽ
    SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);

    // Tính tâm và bán kính của đa giác (lấy bán kính là nửa kích thước nhỏ hơn của rect)
    int cx = rect.x + rect.w / 2;
    int cy = rect.y + rect.h / 2;
    int radius = (rect.w < rect.h ? rect.w : rect.h) / 2;

    // Tính các đỉnh của đa giác
    SDL_Point *vertices = (SDL_Point*)malloc(sizeof(SDL_Point) * sides);
    if (!vertices) return;

    for (int i = 0; i < sides; i++) {
        double angle = 2 * M_PI * i / sides - M_PI / 2; // Trừ M_PI/2 để đỉnh đầu tiên nằm trên cùng
        vertices[i].x = cx + (int)(radius * cos(angle));
        vertices[i].y = cy + (int)(radius * sin(angle));
    }

    // Tính bounding box của đa giác (minY và maxY)
    int polyMinY = vertices[0].y;
    int polyMaxY = vertices[0].y;
    for (int i = 1; i < sides; i++) {
        if (vertices[i].y < polyMinY) polyMinY = vertices[i].y;
        if (vertices[i].y > polyMaxY) polyMaxY = vertices[i].y;
    }

    const int border = 4; // Độ dày viền
    // Tính ngưỡng T dựa theo HP (tương tự như hàm drawRectangle)
    int T = rect.y + (int)(percentHP * (rect.h - 3));

    // Duyệt từng dòng trong bounding box của đa giác
    for (int y = polyMinY; y <= polyMaxY; y++) {
        // Tìm giao điểm của đường ngang tại y với các cạnh của đa giác
        double xInts[2];
        int count = 0;
        for (int i = 0; i < sides; i++) {
            int j = (i + 1) % sides;
            int y1 = vertices[i].y;
            int y2 = vertices[j].y;

            // Kiểm tra nếu dòng y cắt qua cạnh (tránh đếm trùng tại đỉnh)
            if ((y >= y1 && y < y2) || (y >= y2 && y < y1)) {
                double t = (double)(y - y1) / (y2 - y1);
                double x_int = vertices[i].x + t * (vertices[j].x - vertices[i].x);
                if (count < 2) {
                    xInts[count] = x_int;
                    count++;
                }
            }
        }

        // Nếu tìm đủ 2 giao điểm (với đa giác lồi, mỗi dòng cắt qua sẽ có 2 giao điểm)
        if (count == 2) {
            double x1 = xInts[0], x2 = xInts[1];
            if (x1 > x2) {
                double temp = x1;
                x1 = x2;
                x2 = temp;
            }

            // Xét điều kiện để xác định vẽ toàn bộ hay chỉ viền
            // Vẽ toàn bộ nếu dòng thuộc vùng viền trên, dưới hoặc nằm bên dưới ngưỡng T (thể hiện HP)
            if (y < rect.y + border || y > rect.y + rect.h - border - 1 || y > T) {
                SDL_RenderDrawLine(*renderer, (int)round(x1), y, (int)round(x2), y);
            } else {
                // Ở dòng nội bộ, chỉ vẽ viền trái và phải của đoạn giao cắt
                SDL_RenderDrawLine(*renderer, (int)round(x1), y, (int)round(x1) + border - 1, y);
                SDL_RenderDrawLine(*renderer, (int)round(x2) - border + 1, y, (int)round(x2), y);
            }
        }
    }

    free(vertices);
}

void drawGrass(SDL_Renderer** renderer, int originX, int originY, int type) {

    if (type) {
        for (int i = 0; i < GRASS_COUNT; i++) {
            // Sinh góc và bán kính ngẫu nhiên
            float angle = ((float)rand() / RAND_MAX) * 2 * M_PI; // Góc từ 0 đến 2π
            float radius = ((float)rand() / RAND_MAX) * (WORLD_MAP_WIDTH / 2); // Bán kính từ 0 đến 1000

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
    int sizeFont = 24;
    char scoreText[20];

    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(fonts[24], scoreText, textColor);

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

void drawNumberEnemy(SDL_Renderer** renderer, int numberEnemy) {
    char enemyText[20];
    int sizeFont = 24;

    snprintf(enemyText, sizeof(enemyText), "Enemies: %d", numberEnemy);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(fonts[sizeFont], enemyText, textColor);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(*renderer, textSurface);

    SDL_Rect textRect;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    textRect.x = (WINDOW_WIDTH - textRect.w) / 2; // Căn giữa theo chiều ngang
    textRect.y = 10 + textRect.h + 5; // Đặt dưới score, cách 5px

    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(*renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
}

void drawFence(SDL_Renderer** renderer, int originX, int originY, int type) {
    if (type) {
        fences.clear();
        for (int x = 0; x <= WORLD_MAP_WIDTH + 60; x++) {
            for (int y = 0; y <= WORLD_MAP_WIDTH + 60; y++) {
                SDL_Point point = {WORLD_MAP_WIDTH / 2 + 20 - x, WORLD_MAP_WIDTH / 2 + 20 - y};
                if ((WORLD_MAP_WIDTH / 2 + 15) * (WORLD_MAP_WIDTH / 2 + 15) <= point.x * point.x + point.y * point.y &&
                    point.x * point.x + point.y * point.y <= (WORLD_MAP_WIDTH / 2 + 20) * (WORLD_MAP_WIDTH / 2 + 20)) {
                        fences.push_back(point);
                }
            }
        }
    }
    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);

    for (auto fence: fences) {
        int dx = fence.x - originX + WORLD_ORIGIN_X;
        int dy = fence.y - originY + WORLD_ORIGIN_Y;
        if (IsPointInWindow(dx, dy, 10, 10)) {
            SDL_RenderDrawPoint(*renderer, dx, dy);
        }
    }
}

void drawMiniMap(SDL_Renderer** renderer) {

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

    int enemyX, enemyY;

    
    for (auto& [enemyType, vectorEnemy]: enemies) {
        for (auto enemy: vectorEnemy) {
            int enemyX, enemyY;
            enemyX = MINI_MAP_X + static_cast<int>(enemy.rect.x * scale) - WORLD_ORIGIN_X + MINI_MAP_WIDTH / 2;
            enemyY = MINI_MAP_Y + static_cast<int>(enemy.rect.y * scale) - WORLD_ORIGIN_Y + MINI_MAP_HEIGHT / 2;
            if (enemyType == EnemyType::Epic) {
                drawCircle(renderer, enemyX, enemyY, std::min(5, enemy.level + 2), enemyColor);
            } else {
                drawCircle(renderer, enemyX, enemyY, 4, enemyColor);
            }
        }
    }

}

void drawFPS(SDL_Renderer** renderer, int currentFPS) {
    int sizeFont = 12;
    char fpsText[32];

    // Màu chữ (trắng)
    SDL_Color textColor = {255, 255, 255, 255};


    snprintf(fpsText, sizeof(fpsText), "FPS: %d", currentFPS);

    // Tạo surface từ chuỗi text
    SDL_Surface* textSurface = TTF_RenderText_Blended(fonts[sizeFont], fpsText, textColor);

    // Tạo texture từ surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(*renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!textTexture) {
        SDL_Log("Không thể tạo texture từ text: %s", SDL_GetError());
        return;
    }

    // Lấy kích thước texture
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

    SDL_Rect renderQuad = {WINDOW_WIDTH - 128, 4, textWidth, textHeight};
    SDL_RenderCopy(*renderer, textTexture, NULL, &renderQuad);

    SDL_DestroyTexture(textTexture);
}

int renderEndGameScreen(SDL_Renderer** renderer, int score) {
    int sizeFont = 24;
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_RenderClear(*renderer);

    SDL_Color black = {0, 0, 0}; // Định nghĩa màu đen
    
    // Vẽ hộp màu đỏ với chữ "YOU LOSE" và "Score"
    SDL_SetRenderDrawColor(*renderer, 255, 0, 0, 255);
    SDL_Rect messageRect = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 100, 200, 100}; // Tăng chiều cao hộp
    SDL_RenderFillRect(*renderer, &messageRect);

    // Vẽ chữ "YOU LOSE"
    SDL_Surface* messageSurface = TTF_RenderText_Solid(fonts[sizeFont], "YOU LOSE", black);
    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(*renderer, messageSurface);
    SDL_Rect messageTextRect = {WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 90, 160, 30}; // Căn giữa phía trên hộp
    SDL_RenderCopy(*renderer, messageTexture, NULL, &messageTextRect);
    SDL_FreeSurface(messageSurface);
    SDL_DestroyTexture(messageTexture);

    // -------------------- HIỂN THỊ SCORE BÊN TRONG HỘP --------------------
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score); // Tạo chuỗi score

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(fonts[sizeFont], scoreText, black);
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

    SDL_Surface* retrySurface = TTF_RenderText_Solid(fonts[sizeFont], "REPLAY", black);
    SDL_Texture* retryTexture = SDL_CreateTextureFromSurface(*renderer, retrySurface);
    SDL_Rect retryTextRect = {WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 + 30, 70, 30};
    SDL_RenderCopy(*renderer, retryTexture, NULL, &retryTextRect);
    SDL_FreeSurface(retrySurface);
    SDL_DestroyTexture(retryTexture);

    // Vẽ nút "EXIT" màu vàng
    SDL_SetRenderDrawColor(*renderer, 255, 255, 0, 255);
    SDL_Rect exitButton = {WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 + 20, 90, 50};
    SDL_RenderFillRect(*renderer, &exitButton);

    SDL_Surface* exitSurface = TTF_RenderText_Solid(fonts[sizeFont], "EXIT", black);
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
    int ans;

    while (waitingForInput) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waitingForInput = false;
                ans = -1;
                break;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // Kiểm tra nếu bấm vào nút "Chơi lại"
                if (mouseX >= retryButton.x && mouseX <= retryButton.x + retryButton.w &&
                    mouseY >= retryButton.y && mouseY <= retryButton.y + retryButton.h) {

                    ans = 1;
                    waitingForInput = false;
                    break;
                }

                // Kiểm tra nếu bấm vào nút "Thoát game"
                if (mouseX >= exitButton.x && mouseX <= exitButton.x + exitButton.w &&
                    mouseY >= exitButton.y && mouseY <= exitButton.y + exitButton.h) {

                    waitingForInput = false;
                    ans = -1;
                    break;
                }
            }
        }
    }
    return ans;
}

int rendererInitGame(SDL_Renderer** renderer) {
    int sizeFont = 32; // Cỡ chữ lớn hơn
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_RenderClear(*renderer);

    SDL_Color white = {255, 255, 255}; // Màu trắng cho chữ

    // Hiển thị chữ "ACHERO"
    SDL_Surface* titleSurface = TTF_RenderText_Solid(fonts[sizeFont], "ACHERO", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(*renderer, titleSurface);
    SDL_Rect titleRect = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 100, 200, 50};
    
    SDL_RenderCopy(*renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    SDL_Rect startButton = {WINDOW_WIDTH / 2 - 75, WINDOW_HEIGHT / 2, 150, 50};
    SDL_Rect startTextRect = {WINDOW_WIDTH / 2 - 45, WINDOW_HEIGHT / 2 + 10, 90, 30};

    bool waitingForInput = true;
    SDL_Event event;

    while (waitingForInput) {
        Uint32 time = SDL_GetTicks();
        int greenIntensity = 150 + 105 * sin(time * 0.005); // Tạo hiệu ứng nhấp nháy

        SDL_SetRenderDrawColor(*renderer, 0, greenIntensity, 0, 255);
        SDL_RenderFillRect(*renderer, &startButton);

        // Hiển thị chữ "START"
        SDL_Surface* startSurface = TTF_RenderText_Solid(fonts[sizeFont], "START", white);
        SDL_Texture* startTexture = SDL_CreateTextureFromSurface(*renderer, startSurface);
        SDL_RenderCopy(*renderer, startTexture, NULL, &startTextRect);
        SDL_FreeSurface(startSurface);
        SDL_DestroyTexture(startTexture);

        SDL_RenderPresent(*renderer);
        SDL_Delay(50); // Giảm tải CPU và tạo hiệu ứng mượt hơn

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return -1;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (mouseX >= startButton.x && mouseX <= startButton.x + startButton.w &&
                    mouseY >= startButton.y && mouseY <= startButton.y + startButton.h) {
                    return 1; // Bắt đầu game
                }
            }
        }
    }
    return 0;
}


void deInitSDL(SDL_Window** window, SDL_Renderer** renderer) {
    for (int size = 1; size <= 50; size++) {
        TTF_CloseFont(fonts[size]);
    }
    TTF_Quit();
    SDL_DestroyWindow(*window);
    SDL_DestroyRenderer(*renderer);
    SDL_Quit();
}

