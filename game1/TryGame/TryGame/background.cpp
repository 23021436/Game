#include "background.h"
#include <SDL_image.h>
#include <iostream>

Background::Background(SDL_Renderer* renderer, const char* imagePath) {
    backgroundTexture = IMG_LoadTexture(renderer, imagePath);
    if (backgroundTexture == nullptr) {
        std::cerr << "Không thể tải background: " << IMG_GetError() << std::endl;
    }
    scrollY = 0;
}

Background::~Background() {
    if (backgroundTexture != nullptr) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

void Background::update() {
    scrollY += 4; // Tốc độ cuộn (có thể điều chỉnh)
}

void Background::render(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    if (backgroundTexture == nullptr) {
        return;
    }

    // Truy vấn kích thước thực tế của texture
    int texWidth, texHeight;
    SDL_QueryTexture(backgroundTexture, nullptr, nullptr, &texWidth, &texHeight);

    // Tính toán tỷ lệ để giữ nguyên tỷ lệ hình ảnh
    float aspectRatio = (float)texWidth / texHeight;
    int renderWidth = screenWidth;
    int renderHeight = static_cast<int>(screenWidth / aspectRatio);

    // Nếu chiều cao lớn hơn màn hình, điều chỉnh lại
    if (renderHeight < screenHeight) {
        renderHeight = screenHeight;
        renderWidth = static_cast<int>(screenHeight * aspectRatio);
    }

    // Đặt lại scrollY khi background cuộn hết
    if (scrollY >= renderHeight) {
        scrollY = 0;
    }

    // Vẽ background hai lần để tạo hiệu ứng cuộn liên tục
    SDL_Rect srcRect = { 0, 0, texWidth, texHeight };
    SDL_Rect destRect1 = { (screenWidth - renderWidth) / 2, scrollY - renderHeight, renderWidth, renderHeight };
    SDL_Rect destRect2 = { (screenWidth - renderWidth) / 2, scrollY, renderWidth, renderHeight };

    SDL_RenderCopy(renderer, backgroundTexture, &srcRect, &destRect1);
    SDL_RenderCopy(renderer, backgroundTexture, &srcRect, &destRect2);
}