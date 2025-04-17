#include "story.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include <vector>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

Story::Story(SDL_Renderer* renderer)
    : renderer(renderer) {
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    }
    font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 20);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    textColor = { 255, 255, 255 };

    // Nội dung câu chuyện
    lines = {
        "Once upon a time, a panda embarked on an adventure through a mystical forest.",
        "He faced many challenges, met friends, and overcame fears.",
        "In the end, he discovered courage and the true meaning of friendship.",
        "The End."
    };

    // Nút Back giống màn Score
    backRect = { 200, 650, 200, 50 };

    // Tải hình nền
    backgroundTexture = IMG_LoadTexture(renderer, "..\\assets\\img\\ChatGPT Image Apr 15, 2025, 12_30_51 AM.png");
    if (!backgroundTexture) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
    }
}

Story::~Story() {
    if (font) TTF_CloseFont(font);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture); // Hủy texture nền
    TTF_Quit();
}

void Story::renderText(const std::string& text, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Hàm này chia câu văn thành các đoạn nhỏ hơn nếu quá dài
std::vector<std::string> Story::wrapText(const std::string& text, int maxWidth) {
    std::vector<std::string> wrappedText;
    std::string currentLine;

    // Chia câu văn thành các từ
    size_t start = 0;
    while (start < text.length()) {
        size_t end = text.find(' ', start);
        if (end == std::string::npos) end = text.length();

        std::string word = text.substr(start, end - start);
        std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;

        SDL_Surface* surface = TTF_RenderText_Solid(font, testLine.c_str(), textColor);
        if (surface->w <= maxWidth) {
            currentLine = testLine;
        }
        else {
            if (!currentLine.empty()) {
                wrappedText.push_back(currentLine);
            }
            currentLine = word;
        }
        SDL_FreeSurface(surface);

        start = end + 1;  // Chuyển sang từ tiếp theo
    }

    if (!currentLine.empty()) {
        wrappedText.push_back(currentLine);
    }

    return wrappedText;
}

void Story::showStory(bool& isRunning) {
    SDL_Event e;
    bool showing = true;
    Uint32 startTime = SDL_GetTicks();

    while (showing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                isRunning = false;
                showing = false;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                showing = false;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (x >= backRect.x && x <= backRect.x + backRect.w &&
                    y >= backRect.y && y <= backRect.y + backRect.h) {
                    showing = false;
                }
            }
        }

        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Vẽ nền
        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }

        // Vẽ câu chuyện (đảm bảo không tràn ra ngoài màn hình)
        int yOffset = 100;
        const int maxWidth = SCREEN_WIDTH - 40;  // Tính toán lại kích thước tối đa của văn bản (lề 20px mỗi bên)
        for (const std::string& line : lines) {
            std::vector<std::string> wrappedText = wrapText(line, maxWidth);
            for (const std::string& wrappedLine : wrappedText) {
                renderText(wrappedLine, 20, yOffset);
                yOffset += 30;  // Cách nhau giữa các dòng
            }
        }

        // Hiệu ứng hover cho nút Back
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        bool hoveringBack = mx >= backRect.x && mx <= backRect.x + backRect.w &&
            my >= backRect.y && my <= backRect.y + backRect.h;

        SDL_Color backColor = hoveringBack ? SDL_Color{ 150,150,150,255 } : textColor;
        SDL_Surface* backSurf = TTF_RenderText_Solid(font, "Back", backColor);
        SDL_Texture* backTex = SDL_CreateTextureFromSurface(renderer, backSurf);
        SDL_Rect backDst = {
            backRect.x + (backRect.w - backSurf->w) / 2,
            backRect.y + (backRect.h - backSurf->h) / 2,
            backSurf->w, backSurf->h
        };
        SDL_RenderCopy(renderer, backTex, nullptr, &backDst);
        SDL_FreeSurface(backSurf);
        SDL_DestroyTexture(backTex);

        // Hiển thị gợi ý "Press ESC to go back" động
        Uint32 elapsed = SDL_GetTicks() - startTime;
        int offset = static_cast<int>(5 * sin(elapsed * 0.005));
        SDL_Surface* hintSurf = TTF_RenderText_Solid(font, "Press ESC to go back", textColor);
        SDL_Texture* hintTex = SDL_CreateTextureFromSurface(renderer, hintSurf);
        SDL_Rect hintDst = {
            (SCREEN_WIDTH - hintSurf->w) / 2,
            backRect.y + backRect.h + 20 + offset,
            hintSurf->w, hintSurf->h
        };
        SDL_RenderCopy(renderer, hintTex, nullptr, &hintDst);
        SDL_FreeSurface(hintSurf);
        SDL_DestroyTexture(hintTex);

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}
