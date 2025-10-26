#pragma once
#include "Game.h"
#include "SDL.h"

class Player
{
public:
    Player(const char* rightTexture, const char* leftTexture, int x, int y);
    virtual ~Player();
    virtual void Update();
    void Render();

    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setX(int x) { xpos = x; }
    void setY(int y) { ypos = y; }
    int getDirection() const { return currentDir; }

private:
    int xpos;
    int ypos;
    bool isJumping = false;
    int jumpForce = 0;
    int currentDir = 1; 

    SDL_Texture* texLeft;
    SDL_Texture* texRight;
    SDL_Rect srcRect, destRect;
};