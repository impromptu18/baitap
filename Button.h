
#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>

class Button {
public:
    Button();  // Constructor mặc định
    Button(SDL_Rect rect, SDL_Texture* texture);  // Constructor có tham số

    void render(SDL_Renderer* renderer) const;  // Vẽ nút
    bool isClicked(int x, int y) const;         // Kiểm tra nếu nút bị click

    void setRect(SDL_Rect rect);                // Gán vị trí và kích thước
    void setTexture(SDL_Texture* texture);      // Gán texture


private:
    SDL_Rect rect;
    SDL_Texture* texture;
};

#endif
