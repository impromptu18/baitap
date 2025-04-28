#ifndef PIPE_H
#define PIPE_H

#include <SDL.h>

// Pipe.h

class Pipe {
public:
    SDL_Rect top, bottom;
    bool passed;

    Pipe(int x1, int y1, int x2, int y2, int w, int h);

    void update(int speed);
    void render(SDL_Renderer* renderer, SDL_Texture* texture) const;

    bool collidesWith(const SDL_Rect& birdRect, int pipeWidth) const;
    int getX() const;
};


#endif

