#include "Enemy.h"
#include "TextureManager.h"
#include "Game.h"
#include "Player.h"

Enemy::Enemy(const char* rightTexture, const char* leftTexture,
    Player* target, int startX, int startY, float moveSpeed)
    : player(target), x((float)startX), y((float)startY), speed(moveSpeed)
{
    texRight = TextureManager::LoadTexture(rightTexture);
    texLeft = TextureManager::LoadTexture(leftTexture);

    srcRect = { 0, 0, 348, 348 };
    destRect = { startX, startY, srcRect.w / 2, srcRect.h / 2 };
}

Enemy::~Enemy() 
{
    SDL_DestroyTexture(texLeft);
    SDL_DestroyTexture(texRight);
}

void Enemy::Update()
{
    int playerX = player->getX();
    int playerY = player->getY();

    if (playerX > x) 
    {
        x += speed;
        direction = 1;
    }
    else if (playerX < x) 
    {
        x -= speed;
        direction = -1;
    }

    if (playerY > y) 
    {
        y += speed;
    }
    else if (playerY < y) 
    {
        y -= speed;
    }

    destRect.x = static_cast<int>(x);
    destRect.y = static_cast<int>(y);
}

void Enemy::Render()
{
    SDL_Texture* tex = (direction == 1 ? texRight : texLeft);
    SDL_RenderCopy(Game::renderer, tex, &srcRect, &destRect);
}

void Enemy::RespawnAtEdge(int screenWidth, int screenHeight)
{

    if (player->getX() < screenWidth / 2)
        x = screenWidth - destRect.w;
    else
        x = 0;

    y = (float)(rand() % (screenHeight - destRect.h));

    destRect.x = (int)x;
    destRect.y = (int)y;
}
