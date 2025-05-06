#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <fstream>
#include <vector>
#include <string>
#include "Bird.h"
#include "Pipe.h"
#include "Button.h"
#include <iostream>

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
    SDL_Texture* startTexture;
    SDL_Texture* quitTexture;
    SDL_Texture* pauseTexture;
    SDL_Texture* resumeTexture;
    SDL_Texture* groundTexture;
    int groundHeight;

    TTF_Font* font;
    Mix_Music* bgMusic;
    Mix_Chunk* jumpSound;
    Mix_Music* gameOverMusic;
    Mix_Music* menuMusic;
    bool soundOn;  // Biến kiểm tra trạng thái âm thanh (bật/tắt)
    Button soundButton;  // Nút bật/tắt âm thanh
    SDL_Texture* soundOnTexture;  // Texture cho âm thanh bật
    SDL_Texture* soundOffTexture;  // Texture cho âm thanh tắt

    Button startButton, quitButton;
    Button pauseButton;  // Nút pause
    Button resumeButton; // Nút resume
    bool running;

    enum GameState { MENU, PLAYING, GAME_OVER, PAUSED };
    GameState gameState;

    Bird bird;
    std::vector<Pipe> pipes;
    SDL_Event event;
    int score;
    int highScore;
    int loadHighScore();
    void saveHighScore();
    void resetGame();
};

#endif
