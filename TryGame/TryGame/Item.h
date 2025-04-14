#pragma once
#include <SDL.h>
#include <string>

enum class ItemType { PEACH, TRAP };

class Item {
public:
    Item(ItemType type, SDL_Renderer* renderer, const std::string& path, int lane);
    ~Item();

    void update();
    void render(SDL_Renderer* renderer);
    SDL_Rect getRect() const;
    ItemType getType() const;

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
    ItemType type;
};
