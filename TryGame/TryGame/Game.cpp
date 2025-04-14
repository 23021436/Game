#include "Game.h"
#include <SDL_image.h>
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

Game::Game()
    : window(nullptr), renderer(nullptr), isRunning(false),
    background(nullptr), player(nullptr), itemSpawnCounter(0) {}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tải background và player
    background = new Background(renderer, "C:\\Study\\CODE\\TryGame\\assets\\img\\Background.png");
    player = new Player();
    if (!player->init(renderer, "C:\\Study\\CODE\\TryGame\\assets\\img\\Run.png")) {
        std::cerr << "Failed to load player\n";
        return false;
    }

    itemSpawnCounter = 0;

    return true;
}

void Game::run() {
    isRunning = true;
    SDL_Event e;

    while (isRunning) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                isRunning = false;
            player->handleEvent(e);
        }

        background->update();
        player->update();

        // Sinh item định kỳ
        itemSpawnCounter++;
        if (itemSpawnCounter >= 90) {
            int lane = rand() % 3;
            bool isPeach = rand() % 2;

            Item* newItem = new Item(
                isPeach ? ItemType::PEACH : ItemType::TRAP,
                renderer,
                isPeach ? "C:\\Study\\CODE\\TryGame\\assets\\img\\peach.png"
                : "C:\\Study\\CODE\\TryGame\\assets\\img\\trap.png",
                lane
            );
            items.push_back(newItem);
            itemSpawnCounter = 0;
        }

        // Cập nhật & xử lý va chạm
        for (auto it = items.begin(); it != items.end();) {
            (*it)->update();

            SDL_Rect itemRect = (*it)->getRect();
            SDL_Rect playerRect = player->getRect();

            if (SDL_HasIntersection(&itemRect, &playerRect)) {
                if ((*it)->getType() == ItemType::PEACH) {
                    // Không làm gì cả khi ăn phần thưởng
                }
                else {
                    // Thua cuộc khi dính trap
                    isRunning = false;
                    break; // Thoát vòng lặp xử lý item
                }
                delete* it;
                it = items.erase(it);
            }
            else if (itemRect.y > SCREEN_HEIGHT) {
                delete* it;
                it = items.erase(it);
            }
            else {
                ++it;
            }
        }

        // Vẽ màn hình
        SDL_RenderClear(renderer);
        background->render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        for (auto& item : items)
            item->render(renderer);
        player->render(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }
}


void Game::clean() {
    for (auto& item : items)
        delete item;
    items.clear();

    player->clean();
    delete player;
    delete background;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
