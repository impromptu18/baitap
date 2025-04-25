#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Bird.h"
#include "Pipe.h"

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void init();
    void handleEvents();
    void update();
    void render();
    void generatePipe();
    void clean();
    SDL_Texture* loadTexture(const char* path);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture *birdTexture, *pipeTexture, *backgroundTexture, *menuTexture;
    TTF_Font* font;
    bool running;

    enum GameState { MENU, PLAYING, GAME_OVER };
    GameState gameState;

    Bird bird;
    std::vector<Pipe> pipes;
    SDL_Event event;
    int score;
};

#endif
