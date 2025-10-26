#include "Player.h"
#include "TextureManager.h"

Player::Player(const char* rightTexture, const char* leftTexture, int x, int y)
{
    texRight = TextureManager::LoadTexture(rightTexture);
    texLeft = TextureManager::LoadTexture(leftTexture);
    xpos = x;
    ypos = y;
    currentDir = 1; 
    srcRect = { 0, 0, 348, 348 };
    destRect = { xpos, ypos, srcRect.w / 2, srcRect.h / 2 };
}

Player::~Player() {}

void Player::Update()
{
    int speed = 3;
    const int gravity = 1;
    const int floorY = 540; 

    if (Game::isKeyPressed(SDL_SCANCODE_LSHIFT))
        speed = 4;

    if (Game::isKeyPressed(SDL_SCANCODE_A)) 
    {
        xpos -= speed;
        currentDir = -1;
    }
    if (Game::isKeyPressed(SDL_SCANCODE_D)) 
    {
        xpos += speed;
        currentDir = 1; 
    }

    if (!isJumping) { 
        if (Game::isKeyPressed(SDL_SCANCODE_SPACE)) 
        {
            isJumping = true;
            jumpForce = -15; 
        }
    }

    if (isJumping) 
    {
        ypos += jumpForce;
        jumpForce += gravity;
    }

    if (ypos >= floorY) 
    {
        ypos = floorY;
        isJumping = false;
        jumpForce = 0;
    }

    destRect.x = xpos;
    destRect.y = ypos;
}

void Player::Render()
{
    SDL_Texture* currentTexture = (currentDir == -1) ? texLeft : texRight;
    SDL_RenderCopy(Game::renderer, currentTexture, &srcRect, &destRect);
}