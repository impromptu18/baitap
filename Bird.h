#ifndef BIRD_H
#define BIRD_H

#include <SDL.h>

class Bird {
public:
    void init(int x, int y, int w, int h);
    void update(float gravity);
    void jump(float strength);
    void render(SDL_Renderer* renderer, SDL_Texture* texture);
    SDL_Rect getRect() const;

private:
    SDL_Rect rect;
    float velocity;
    float angle;
};

#endif
