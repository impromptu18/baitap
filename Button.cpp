
#include "Button.h"

Button::Button() {
    rect = {0, 0, 0, 0};
    texture = nullptr;
}

Button::Button(SDL_Rect r, SDL_Texture* tex) {
    rect = r;
    texture = tex;
}

void Button::render(SDL_Renderer* renderer) const {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}

bool Button::isClicked(int x, int y) const {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void Button::setRect(SDL_Rect r) {
    rect = r;
}

void Button::setTexture(SDL_Texture* tex) {
    texture = tex;
}


