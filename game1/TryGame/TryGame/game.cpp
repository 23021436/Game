#include "game.h"
#include "story.h"
#include "menu.h"
#include "setting.h"
#include "score.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 750;

Game::Game()
    : window(nullptr), renderer(nullptr), isRunning(false),
    background(nullptr), player(nullptr), itemSpawnCounter(0) {
}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
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

    background = new Background(renderer, "..\\assets\\img\\Background.png");
    player = new Player();
    if (!player->init(renderer, "..\\assets\\img\\Run.png")) {
        std::cerr << "Failed to load player\n";
        return false;
    }

    itemSpawnCounter = 0;

    return true;
}

void Game::run() {
    isRunning = true;
    SDL_Event e;

    Menu menu(renderer);

    while (isRunning) {
        bool startGame = false;
        int menuOption = -1;

        // Vòng lặp menu
        while (isRunning && !startGame && menuOption == -1) {
            while (SDL_PollEvent(&e)) {
                menu.handleEvent(e, isRunning, startGame, menuOption);
            }

            SDL_RenderClear(renderer);
            menu.render(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if (!isRunning) break;

        // Xử lý lựa chọn từ menu
        switch (menuOption) {
        case MENU_PLAY:
            startGame = true;
            break;
        case MENU_SCORE:
            showHighScores();
            continue;
        case MENU_SETTING:
            showSettings();  // sau khi chạy setting xong, tiếp tục vòng lặp menu
            continue;
        case MENU_STORY:
            showStory();
            continue;
        case MENU_EXIT:
            isRunning = false;
            break;
        default:
            continue;
        }

        if (!startGame) continue;

        // --- Vòng lặp game chính ---
        while (isRunning) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT)
                    isRunning = false;
                player->handleEvent(e);
            }

            background->update();
            player->update();

            itemSpawnCounter++;
            if (itemSpawnCounter >= 90) {
                int lane = rand() % 3;
                bool isPeach = rand() % 2;

                Item* newItem = new Item(
                    isPeach ? ItemType::PEACH : ItemType::TRAP,
                    renderer,
                    isPeach ? "..\\assets\\img\\peach.png" : "..\\assets\\img\\trap.png",
                    lane
                );
                items.push_back(newItem);
                itemSpawnCounter = 0;
            }

            for (auto it = items.begin(); it != items.end();) {
                (*it)->update();
                SDL_Rect itemRect = (*it)->getRect();
                SDL_Rect playerRect = player->getRect();

                if (SDL_HasIntersection(&itemRect, &playerRect)) {
                    if ((*it)->getType() == ItemType::PEACH) {
                        currentScore += 10; // hoặc tuỳ điểm số bạn cộng khi ăn peach
                    }
                    else {
                        // Va chạm trap => kết thúc game và lưu điểm
                        ScoreManager scoreManager;
                        scoreManager.addScore(currentScore);

                        isRunning = false;
                        break;
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

            SDL_RenderClear(renderer);
            background->render(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
            for (auto& item : items)
                item->render(renderer);
            player->render(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    }
}

void Game::showHighScores() {
    ScoreManager scoreManager;
    scoreManager.showHighScores(renderer, isRunning);
}

void Game::showSettings() {
    Setting setting(renderer);
    setting.run();
}

void Game::showStory() {
Story story(renderer);
story.showStory(isRunning);
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
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}