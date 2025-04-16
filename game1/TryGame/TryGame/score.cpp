#include "score.h"
#include <SDL_ttf.h>
#include <SDL.h>
#include <fstream>
#include <algorithm>
#include <iostream>

ScoreManager::ScoreManager(const std::string& filename)
    : filename(filename) {
    loadScores();
}

void ScoreManager::loadScores() {
    scores.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open score file: " << filename << std::endl;
        return;
    }

    int score;
    while (file >> score) {
        scores.push_back(score);
    }
    file.close();
    sortScores();
}

void ScoreManager::saveScores() const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not write to score file: " << filename << std::endl;
        return;
    }

    for (int score : scores) {
        file << score << "\n";
    }

    file.close();
}

void ScoreManager::addScore(int score) {
    scores.push_back(score);
    sortScores();
    if (scores.size() > MAX_SCORES) {
        scores.resize(MAX_SCORES);
    }
    saveScores();
}

const std::vector<int>& ScoreManager::getHighScores() const {
    return scores;
}

void ScoreManager::sortScores() {
    std::sort(scores.begin(), scores.end(), std::greater<int>());
}

// -------------------------------
// Hiển thị điểm số cao trong cửa sổ SDL
// -------------------------------
void ScoreManager::showHighScores(SDL_Renderer* renderer, bool& isRunning) {
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return;
    }

    TTF_Font* font = TTF_OpenFont("..\\assets\\fonts\\PressStart2P-Regular.ttf", 20);
    if (!font) {
        std::cerr << "Failed to load font for scores: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Event e;
    bool showing = true;
    SDL_Color white = { 255, 255, 255 };

    SDL_Rect backRect = { 200, 650, 200, 50 }; // Nút "Back"

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
                int x = e.button.x;
                int y = e.button.y;
                if (x >= backRect.x && x <= backRect.x + backRect.w &&
                    y >= backRect.y && y <= backRect.y + backRect.h) {
                    showing = false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // nền đen
        SDL_RenderClear(renderer);

        // Hiển thị điểm số
        for (size_t i = 0; i < scores.size(); ++i) {
            std::string scoreText = std::to_string(i + 1) + ". Score: " + std::to_string(scores[i]);
            SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect destRect = { 100, 100 + static_cast<int>(i) * 40, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        // Vẽ nút "Back"
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &backRect);

        SDL_Surface* backSurface = TTF_RenderText_Solid(font, "Back", white);
        SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
        SDL_Rect textRect = {
            backRect.x + 10,
            backRect.y + 5,
            backSurface->w,
            backSurface->h
        };
        SDL_RenderCopy(renderer, backTexture, nullptr, &textRect);
        SDL_FreeSurface(backSurface);
        SDL_DestroyTexture(backTexture);

        // Hiển thị dòng "Press ESC to go back"
        std::string escHint = "Press ESC to go back";
        SDL_Surface* hintSurface = TTF_RenderText_Solid(font, escHint.c_str(), white);
        SDL_Texture* hintTexture = SDL_CreateTextureFromSurface(renderer, hintSurface);
        SDL_Rect hintRect = {
            (600 - hintSurface->w) / 2,
            backRect.y + backRect.h + 20,
            hintSurface->w,
            hintSurface->h
        };
        SDL_RenderCopy(renderer, hintTexture, nullptr, &hintRect);
        SDL_FreeSurface(hintSurface);
        SDL_DestroyTexture(hintTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
}
