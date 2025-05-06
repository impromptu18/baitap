#include "Game.h"
#include "Button.h"
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
    soundOn = true;
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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    bgMusic = Mix_LoadMUS("music.mp3");
    jumpSound = Mix_LoadWAV("jump.wav");
    gameOverMusic = Mix_LoadMUS("gameover.wav");
    menuMusic = Mix_LoadMUS("menu_music.mp3");

    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    backgroundTexture = loadTexture("background.png");
    birdTexture = loadTexture("bird.png");
    pipeTexture = loadTexture("pipe.png");
    menuTexture = loadTexture("menu.png");

    startTexture = loadTexture("start_button.png");
    font = TTF_OpenFont("arialbd.ttf", 28);
     soundOnTexture = loadTexture("soundon.png");  // Nạp hình ảnh âm thanh bật
    soundOffTexture = loadTexture("soundoff.png");  // Nạp hình ảnh âm thanh tắt

    // Căn giữa nút start
    startButton = {{SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 , 200, 100}, startTexture};
    highScore = loadHighScore();
    soundButton = {{SCREEN_WIDTH - 60, 20, 40, 40}, soundOnTexture};  // Nút góc phải trên
    Mix_PlayMusic(menuMusic, -1);

    srand(static_cast<unsigned int>(time(nullptr)));
}

void Game::handleEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (gameState == MENU) {
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (startButton.isClicked(mouseX, mouseY)) {
                    gameState = PLAYING;
                    bird.init(100, 250, 60, 50);
                    pipes.clear();
                    score = 0;

                    Mix_HaltMusic();
                    Mix_PlayMusic(bgMusic, -1);
                }
                if (soundButton.isClicked(mouseX, mouseY)) {
                    soundOn = !soundOn;  // Chuyển đổi trạng thái âm thanh

                    if (soundOn) {
                        soundButton.setTexture(soundOnTexture);  // Đổi hình ảnh nút sang âm thanh bật
                        Mix_VolumeMusic(MIX_MAX_VOLUME);  // Bật âm thanh
                        Mix_VolumeChunk(jumpSound, MIX_MAX_VOLUME);  // Bật âm thanh nhảy
                    } else {
                        soundButton.setTexture(soundOffTexture);  // Đổi hình ảnh nút sang âm thanh tắt
                        Mix_VolumeMusic(0);  // Tắt âm thanh
                        Mix_VolumeChunk(jumpSound, 0);  // Tắt âm thanh nhảy
                    }
                }
            }

            // Kiểm tra phím Space trong menu
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                gameState = PLAYING;
                bird.init(100, 250, 60, 50);
                pipes.clear();
                score = 0;

                Mix_HaltMusic();
                Mix_PlayMusic(bgMusic, -1);
            }

        } else if (gameState == PLAYING) {
            // Kiểm tra phím Space trong khi chơi
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                bird.jump(JUMP_STRENGTH);
                Mix_PlayChannel(-1, jumpSound, 0);
            }

            // Kiểm tra chuột trong khi chơi
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                bird.jump(JUMP_STRENGTH);
                Mix_PlayChannel(-1, jumpSound, 0);
            }

        } else if (gameState == GAME_OVER) {
            // Kiểm tra chuột trong màn hình Game Over
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                resetGame();  // Gọi hàm resetGame để bắt đầu lại trò chơi
            }

            // Kiểm tra phím Space trong màn hình Game Over để quay lại MENU
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                resetGame();  // Gọi hàm resetGame để bắt đầu lại trò chơi
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
        // Cập nhật và lưu điểm cao nhất nếu cần
        if (score > highScore) {
            highScore = score;  // Cập nhật điểm cao nhất
            saveHighScore();    // Lưu điểm cao nhất vào tệp
        }

        gameState = GAME_OVER;

        // Render điểm số
        SDL_Color textColor = {255, 255, 255};
        std::string scoreText = "Score: " + std::to_string(score);
        std::string highScoreText = "High Score: " + std::to_string(highScore);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 2 + 50, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        std::string instructionText = "Press Space to Restart";
        SDL_Surface* instructionSurface = TTF_RenderText_Solid(font, instructionText.c_str(), textColor);
        SDL_Texture* instructionTexture = SDL_CreateTextureFromSurface(renderer, instructionSurface);
        SDL_Rect instructionRect = {SCREEN_WIDTH / 2 - instructionSurface->w / 2, SCREEN_HEIGHT / 2 + 100, instructionSurface->w, instructionSurface->h};
        SDL_RenderCopy(renderer, instructionTexture, NULL, &instructionRect);
        SDL_FreeSurface(instructionSurface);
        SDL_DestroyTexture(instructionTexture);
        Mix_HaltMusic();
        Mix_PlayMusic(gameOverMusic, 1);
        return;
    }

    for (auto& pipe : pipes) {
        pipe.update(PIPE_SPEED);

        if (pipe.collidesWith(bird.getRect(), PIPE_WIDTH)) {
            gameState = GAME_OVER;
            if (score > highScore) {
                highScore = score;  // Cập nhật điểm cao nhất
                saveHighScore();    // Lưu điểm cao nhất vào tệp
            }

            Mix_HaltMusic();
            Mix_PlayMusic(gameOverMusic, 1);
            return;
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
        startButton.render(renderer);
        soundButton.render(renderer);
    } else if (gameState == PLAYING) {
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

        SDL_Color textColor = {255, 255, 255};
        std::string gameOverText = "Game Over!";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 3, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        std::string scoreText = "Score: " + std::to_string(score);
        textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 2, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        std::string highScoreText = "High Score: " + std::to_string(highScore);
        textSurface = TTF_RenderText_Solid(font, highScoreText.c_str(), textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 2 + 50, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Vẽ dòng "Press Space to Restart"
        std::string instructionText = "Press Space to Restart";
        textSurface = TTF_RenderText_Solid(font, instructionText.c_str(), textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 2 + 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}

int Game::loadHighScore() {
    std::ifstream file("highscore.txt");
    int hs = 0;
    if (file.is_open()) {
        file >> hs;
        file.close();
    }
    return hs;
}

void Game::saveHighScore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}
void Game::resetGame() {
    gameState = MENU;
    score = 0;
    pipes.clear();
    bird.init(100, 250, 60, 50);
    Mix_HaltMusic();
    Mix_PlayMusic(menuMusic, -1);

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
    Mix_FreeMusic(menuMusic);
    Mix_FreeMusic(bgMusic);
    Mix_FreeMusic(gameOverMusic);
    Mix_CloseAudio();
    SDL_DestroyTexture(startTexture);
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
