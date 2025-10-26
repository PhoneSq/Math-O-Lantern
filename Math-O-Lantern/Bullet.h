#pragma once
#include <SDL.h>

class Bullet 
{
public:
    Bullet(const char* rightTexture, const char* leftTexture,
        int startX, int startY, int direction, float speed);
    ~Bullet() = default;

    void Update();
    void Render();
    bool IsOffScreen(int screenWidth, int screenHeight) const;

    const SDL_Rect& GetRect() const { return rect; }

private:
    SDL_Texture* texLeft;
    SDL_Texture* texRight;

    float x, y;
    int direction;
    float speed;
    SDL_Rect rect;
};

