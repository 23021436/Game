#include "setting.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

Setting::Setting(SDL_Renderer* renderer) : renderer(renderer), isRunning(true), musicVolume(50), sfxVolume(50) {
    font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 24);
    textColor = { 255, 255, 255 };

    int sliderWidth = 300;
    int sliderHeight = 20;
    int sliderX = (SCREEN_WIDTH - sliderWidth) / 2;

    musicSlider = { sliderX, 200, sliderWidth, sliderHeight };
    sfxSlider = { sliderX, 300, sliderWidth, sliderHeight };

    int backWidth = 100;
    int backHeight = 40;
    int backX = (SCREEN_WIDTH - backWidth) / 2;
    int backY = SCREEN_HEIGHT - 160; // <-- DỜI CAO HƠN

    backButton = { backX, backY, backWidth, backHeight };

    backgroundTexture = nullptr;
    if (!loadBackground("..\\assets\\img\\ChatGPT Image Apr 15, 2025, 12_30_51 AM.png")) {
        std::cerr << "Failed to load background image.\n";
    }
}

Setting::~Setting() {
    TTF_CloseFont(font);
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
}

bool Setting::loadBackground(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return false;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!backgroundTexture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Setting::renderText(const std::string& text, int x, int y) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect dstRect = { x, y, textSurface->w, textSurface->h };
    if (x == -1) {
        dstRect.x = (SCREEN_WIDTH - textSurface->w) / 2;
    }

    SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Setting::renderSlider(SDL_Rect slider, int value) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &slider);

    SDL_Rect handle = { slider.x + (value * slider.w / 100) - 10, slider.y - 5, 20, slider.h + 10 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &handle);
}

void Setting::run() {
    SDL_Event e;
    while (isRunning) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        bool hoveringBack = mouseX >= backButton.x && mouseX <= backButton.x + backButton.w &&
            mouseY >= backButton.y && mouseY <= backButton.y + backButton.h;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                isRunning = false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                if (x >= musicSlider.x && x <= musicSlider.x + musicSlider.w &&
                    y >= musicSlider.y && y <= musicSlider.y + musicSlider.h) {
                    draggingMusic = true;
                }
                else if (x >= sfxSlider.x && x <= sfxSlider.x + sfxSlider.w &&
                    y >= sfxSlider.y && y <= sfxSlider.y + sfxSlider.h) {
                    draggingEffect = true;
                }
                else if (x >= backButton.x && x <= backButton.x + backButton.w &&
                    y >= backButton.y && y <= backButton.y + backButton.h) {
                    isRunning = false;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                draggingMusic = false;
                draggingEffect = false;
            }
            else if (e.type == SDL_MOUSEMOTION) {
                int x = e.motion.x;
                if (draggingMusic) {
                    musicVolume = (x - musicSlider.x) * 100 / musicSlider.w;
                    if (musicVolume < 0) musicVolume = 0;
                    if (musicVolume > 100) musicVolume = 100;
                }
                else if (draggingEffect) {
                    sfxVolume = (x - sfxSlider.x) * 100 / sfxSlider.w;
                    if (sfxVolume < 0) sfxVolume = 0;
                    if (sfxVolume > 100) sfxVolume = 100;
                }
            }
        }

        // Draw background
        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }

        renderText("Music Volume", -1, musicSlider.y - 30);
        renderSlider(musicSlider, musicVolume);

        renderText("SFX Volume", -1, sfxSlider.y - 30);
        renderSlider(sfxSlider, sfxVolume);

        // Vẽ nút back với hiệu ứng hover
        if (hoveringBack) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150); // mờ hơn khi hover
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &backButton);
        renderText("Back", -1, backButton.y + 5);

        SDL_RenderPresent(renderer);
    }
}
