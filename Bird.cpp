
#include "Bird.h"
#include <algorithm>

void Bird::init(int x, int y, int w, int h) {
    rect = {x, y, w, h};
    velocity = 0;
    angle = 0;
}

void Bird::update(float gravity) {
    velocity += gravity;
    rect.y += static_cast<int>(velocity);
    angle = (velocity < 0) ? -30.0f : std::min(30.0f, angle + 2.0f);
}

void Bird::jump(float strength) {
    velocity = strength;
}

void Bird::render(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Point center = {rect.w / 2, rect.h / 2};
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, &center, SDL_FLIP_NONE);
}

SDL_Rect Bird::getRect() const {
    return rect;
}
