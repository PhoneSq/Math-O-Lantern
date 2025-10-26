#include "Bullet.h"
#include "Game.h"
#include "TextureManager.h"

Bullet::Bullet(const char* rightTexture, const char* leftTexture,
    int startX, int startY, int direction, float speed)
    : x((float)startX), y((float)startY), direction(direction), speed(speed)
{
    texRight = TextureManager::LoadTexture(rightTexture);
    texLeft = TextureManager::LoadTexture(leftTexture);

    rect = { startX, startY, 100, 100 }; 
}

void Bullet::Update()
{
    x += direction * speed;
    rect.x = (int)x;
    rect.y = (int)y;
}

void Bullet::Render()
{
    SDL_Texture* tex = (direction == 1 ? texRight : texLeft);
    SDL_RenderCopy(Game::renderer, tex, nullptr, &rect);
}

bool Bullet::IsOffScreen(int screenWidth, int screenHeight) const
{
    return rect.x < -rect.w || rect.x > screenWidth;
}
