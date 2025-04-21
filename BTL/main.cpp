#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;
const float GRAVITY = 0.5f;
const float JUMP_STRENGTH = -8.0f;
const int PIPE_WIDTH = 60;
const int PIPE_GAP = 180;
const int PIPE_SPEED = 3;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* birdTexture = nullptr;
SDL_Texture* pipeTexture = nullptr;

struct Bird {
    SDL_Rect rect;
    float velocity;
    float angle;
} bird;

struct Pipe {
    SDL_Rect top, bottom;
};

std::vector<Pipe> pipes;
bool running = true;
SDL_Event event;

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Failed to load image! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    birdTexture = loadTexture("bird.png");
    pipeTexture = loadTexture("pipe.png");
    bird.rect = {100, 250, 70, 60}; // Chim lớn hơn
    bird.velocity = 0;
    bird.angle = 0;
    srand(time(0));
}

void handleEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
            bird.velocity = JUMP_STRENGTH;
        }
    }
}

void generatePipe() {
    int gapY = rand() % (SCREEN_HEIGHT - PIPE_GAP - 100) + 50;
    Pipe newPipe = {{SCREEN_WIDTH, gapY - 400, PIPE_WIDTH, 400}, {SCREEN_WIDTH, gapY + PIPE_GAP, PIPE_WIDTH, 400}};
    pipes.push_back(newPipe);
}

void update() {
    bird.velocity += GRAVITY;
    bird.rect.y += static_cast<int>(bird.velocity);

    // Cập nhật góc xoay của chim dựa vào vận tốc
    if (bird.velocity < 0) {
        bird.angle = -30; // Chim ngước lên khi nhảy
    } else {
        bird.angle = std::min(30.0f, bird.angle + 2.0f); // Chim cúi xuống khi rơi
    }

    if (bird.rect.y > SCREEN_HEIGHT - bird.rect.h) {
        bird.rect.y = SCREEN_HEIGHT - bird.rect.h;
        running = false;
    }

    for (auto& pipe : pipes) {
        pipe.top.x -= PIPE_SPEED;
        pipe.bottom.x -= PIPE_SPEED;

        // Kiểm tra va chạm
        if ((bird.rect.x + bird.rect.w > pipe.top.x && bird.rect.x < pipe.top.x + PIPE_WIDTH &&
             (bird.rect.y < pipe.top.y + pipe.top.h || bird.rect.y + bird.rect.h > pipe.bottom.y))) {
            running = false;
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
    SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255);
    SDL_RenderClear(renderer);

    SDL_Point center = {bird.rect.w / 2, bird.rect.h / 2};
    SDL_RenderCopyEx(renderer, birdTexture, NULL, &bird.rect, bird.angle, &center, SDL_FLIP_NONE);

    for (const auto& pipe : pipes) {
        SDL_RenderCopy(renderer, pipeTexture, NULL, &pipe.top);
        SDL_RenderCopy(renderer, pipeTexture, NULL, &pipe.bottom);
    }
    SDL_RenderPresent(renderer);
}

void clean() {
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
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
