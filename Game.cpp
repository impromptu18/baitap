#include "Game.h"
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;
const float GRAVITY = 0.5f;
const float JUMP_STRENGTH = -8.0f;
const int PIPE_WIDTH = 60;
const int PIPE_GAP = 180;
const int PIPE_SPEED = 3;

Game::Game() {
    window = nullptr;
    renderer = nullptr;
    running = true;
    score = 0;
    gameState = MENU;
}

Game::~Game() {
    clean();
}

SDL_Texture* Game::loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Game::init() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    backgroundTexture = loadTexture("background.png");
    birdTexture = loadTexture("bird.png");
    pipeTexture = loadTexture("pipe.png");
    menuTexture = loadTexture("menu.png");

    font = TTF_OpenFont("arialbd.ttf", 28);

    bird.init(100, 250, 60, 50);

    srand(static_cast<unsigned int>(time(nullptr)));
}

void Game::handleEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;

        if (gameState == MENU) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                gameState = PLAYING;
                bird.init(100, 250, 60, 50);
                pipes.clear();
                score = 0;
            }
        } else if (gameState == PLAYING) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                bird.jump(JUMP_STRENGTH);
            }
        } else if (gameState == GAME_OVER) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                gameState = MENU;
                score = 0;
            }
        }
    }
}

void Game::generatePipe() {
    int gapY = rand() % (SCREEN_HEIGHT - PIPE_GAP - 100) + 50;
    pipes.emplace_back(SCREEN_WIDTH, gapY - 400, SCREEN_WIDTH, gapY + PIPE_GAP, PIPE_WIDTH, 400);
}

void Game::update() {
    if (gameState != PLAYING) return;

    bird.update(GRAVITY);

    if (bird.getRect().y > SCREEN_HEIGHT - bird.getRect().h) {
        gameState = GAME_OVER;
    }

    for (auto& pipe : pipes) {
        pipe.update(PIPE_SPEED);

        if (pipe.collidesWith(bird.getRect(), PIPE_WIDTH)) {
            gameState = GAME_OVER;
        }

        if (!pipe.passed && bird.getRect().x > pipe.getX() + PIPE_WIDTH) {
            score++;
            pipe.passed = true;
        }
    }

    if (pipes.empty() || pipes.back().getX() < SCREEN_WIDTH - 300) {
        generatePipe();
    }

    if (!pipes.empty() && pipes.front().getX() + PIPE_WIDTH < 0) {
        pipes.erase(pipes.begin());
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    if (gameState == MENU) {
        SDL_Rect menuRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, menuTexture, NULL, &menuRect);
    } else if (gameState == PLAYING){
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);

        bird.render(renderer, birdTexture);

        for (const auto& pipe : pipes) {
            pipe.render(renderer, pipeTexture);
        }

        SDL_Color textColor = {255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, 20, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    } else if (gameState == GAME_OVER) {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);

        // Hiển thị Game Over
        SDL_Color textColor = {255, 255, 255};
        std::string gameOverText = "Game Over!";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 3, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Hiển thị điểm số cuối cùng
        std::string scoreText = "Score: " + std::to_string(score);
        textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 2, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    TTF_CloseFont(font);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void Game::run() {
    init();
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

