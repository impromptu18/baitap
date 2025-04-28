#include "Pipe.h"

Pipe::Pipe(int x1, int y1, int x2, int y2, int w, int h) {
    top = {x1, y1, w, h};
    bottom = {x2, y2, w, h};
    passed = false;
}

void Pipe::update(int speed) {
    top.x -= speed;
    bottom.x -= speed;
}

void Pipe::render(SDL_Renderer* renderer, SDL_Texture* texture) const {
    SDL_RenderCopy(renderer, texture, NULL, &top);
    SDL_RenderCopy(renderer, texture, NULL, &bottom);
}

bool Pipe::collidesWith(const SDL_Rect& birdRect, int pipeWidth) const {
    return SDL_HasIntersection(&birdRect, &top) || SDL_HasIntersection(&birdRect, &bottom);
}

int Pipe::getX() const {
    return top.x;
}
