#pragma once
#include <SDL.h>

class Player;

class Enemy {
public:
    Enemy(const char* rightTexture, const char* leftTexture,
        Player* target, int startX, int startY, float moveSpeed);

    ~Enemy();

    void Update();
    void Render();

    void RespawnAtEdge(int screenWidth, int screenHeight);

    const SDL_Rect& GetRect() const { return destRect; }

private:
    float x, y;
    float speed;

    Player* player;   

    SDL_Texture* texLeft;
    SDL_Texture* texRight;

    int direction = 1;

    SDL_Rect srcRect, destRect;
};

