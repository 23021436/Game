#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "Background.h"
#include "Player.h"
#include "Item.h"

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();
    void clean();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Background* background;
    Player* player;

    std::vector<Item*> items;
    int itemSpawnCounter;
};

#endif // GAME_H
