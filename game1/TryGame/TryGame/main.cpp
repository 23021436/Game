#define SDL_MAIN_HANDLED
#include "game.h"
#include "setting.h"
int main(int argc, char* argv[]) {
    Game game;
    if (game.init("Panda Adventure", 600, 750)) {
        game.run();
    }
    return 0;
}
