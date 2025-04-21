#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Constants
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;
const float GRAVITY = 0.5f;
const float JUMP_STRENGTH = -8.0f;
const int PIPE_WIDTH = 60;
const int PIPE_GAP = 180;
const int PIPE_SPEED = 3;

enum GameState { MENU, PLAYING };
GameState gameState = MENU;


SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* birdTexture = nullptr;
SDL_Texture* pipeTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* menuTexture = nullptr;

TTF_Font* font = nullptr;
int score = 0;

struct Bird {
    SDL_Rect rect;
    float velocity;
    float angle;
} bird;

struct Pipe {
    SDL_Rect top, bottom;
    bool passed = false;
};

std::vector<Pipe> pipes;
bool running = true;
SDL_Event event;

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Failed to load " << path << " | IMG_Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void init() {
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

    bird.rect = {100, 250, 60, 50};
    bird.velocity = 0;
    bird.angle = 0;

    srand(static_cast<unsigned int>(time(nullptr)));
}

void handleEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;

        if (gameState == MENU) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                gameState = PLAYING;
                bird.velocity = 0;
                bird.rect.y = 250;
                pipes.clear();
                score = 0;
            }
        } else if (gameState == PLAYING) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                bird.velocity = JUMP_STRENGTH;
            }
        }
    }
}

void generatePipe() {
    int gapY = rand() % (SCREEN_HEIGHT - PIPE_GAP - 100) + 50;
    Pipe newPipe = {
        {SCREEN_WIDTH, gapY - 400, PIPE_WIDTH, 400},
        {SCREEN_WIDTH, gapY + PIPE_GAP, PIPE_WIDTH, 400},
        false
    };
    pipes.push_back(newPipe);
}
void update() {
    if (gameState != PLAYING) return;

    bird.velocity += GRAVITY;
    bird.rect.y += static_cast<int>(bird.velocity);

    bird.angle = (bird.velocity < 0) ? -30.0f : std::min(30.0f, bird.angle + 2.0f);

    if (bird.rect.y > SCREEN_HEIGHT - bird.rect.h) {
        bird.rect.y = SCREEN_HEIGHT - bird.rect.h;
        running = false;
    }

    for (auto& pipe : pipes) {
        pipe.top.x -= PIPE_SPEED;
        pipe.bottom.x -= PIPE_SPEED;


        if ((bird.rect.x + bird.rect.w > pipe.top.x && bird.rect.x < pipe.top.x + PIPE_WIDTH &&
            (bird.rect.y < pipe.top.y + pipe.top.h || bird.rect.y + bird.rect.h > pipe.bottom.y))) {
            running = false;
        }

        if (!pipe.passed && bird.rect.x > pipe.top.x + PIPE_WIDTH) {
            score++;
            pipe.passed = true;
        }
    }

    if (pipes.empty() || pipes.back().top.x < SCREEN_WIDTH - 300) {
        generatePipe();
    }

    if (!pipes.empty() && pipes.front().top.x + PIPE_WIDTH < 0) {
        pipes.erase(pipes.begin());
    }
}

void render() {
    SDL_RenderClear(renderer);

    if (gameState == MENU) {
        SDL_Rect menuRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, menuTexture, NULL, &menuRect);
    } else {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);

        SDL_Point center = {bird.rect.w / 2, bird.rect.h / 2};
        SDL_RenderCopyEx(renderer, birdTexture, NULL, &bird.rect, bird.angle, &center, SDL_FLIP_NONE);

        for (const auto& pipe : pipes) {
            SDL_RenderCopy(renderer, pipeTexture, NULL, &pipe.top);
            SDL_RenderCopy(renderer, pipeTexture, NULL, &pipe.bottom);
        }


        SDL_Color textColor = {255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, 20, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}

// Cleanup
void clean() {
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

int main(int argc, char* argv[]) {
    init();
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
    clean();
    return 0;
}
