#include <SDL.h>
#include <SDL_image.h>
#include "Background.h"

int main(int argc, char* argv[]) {
    const int SCREEN_WIDTH = 600;
    const int SCREEN_HEIGHT = 800;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Panda Adventure Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load background từ ảnh bạn vừa cung cấp
    Background background(renderer, "C:\\Study\\CODE\\TryGame\\assets\\img\\Background.png");

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        background.update(); // Cập nhật vị trí cuộn
        background.render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}