#pragma once
#include <SDL.h>

class Background {
public:
    Background(SDL_Renderer* renderer, const char* imagePath);
    ~Background();

    void update();
    void render(SDL_Renderer* renderer, int screenWidth, int screenHeight);

private:
    SDL_Texture* backgroundTexture;
    int scrollY;
};