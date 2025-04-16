#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "background.h"
#include "player.h"
#include "item.h"
#include "score.h" // <-- thêm dòng này

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();
    void clean();
    void showHighScores();
    void showSettings();
    void showStory();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Background* background;
    Player* player;

    std::vector<Item*> items;
    int itemSpawnCounter;

    int currentScore; // <-- thêm dòng này
};

#endif // GAME_H
