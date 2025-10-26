#include "Game.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Map.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b, int shrink)
{
    SDL_Rect r1 = a;
    SDL_Rect r2 = b;

    r1.x += shrink;
    r1.w -= shrink * 2;
    r1.h -= shrink * 2;

    r2.x += shrink;
    r2.w -= shrink * 2;
    r2.h -= shrink * 2;

    return (r1.x < r2.x + r2.w &&
        r1.x + r1.w > r2.x &&
        r1.y < r2.y + r2.h &&
        r1.y + r1.h > r2.y);
}

Player* player1;
std::vector<Enemy*> rightEnemies;
std::vector<Enemy*> leftEnemies;
Map* map;
std::vector<Bullet*> bullets;

SDL_Renderer* Game::renderer = nullptr;
bool Game::keys[SDL_NUM_SCANCODES] = { false };
GameState Game::currentState = MENU;
int Game::playerLives = 3;

Game::Game() : selectedAnswer(0), quizTimer(0.0f) 
{
    srand(static_cast<unsigned>(time(nullptr)));
}

Game::~Game() {}

void Game::init(const char* title, int width, int height, bool fullscreen)
{
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        std::cout << "SDL Initialized!" << std::endl;
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
        isRunning = true;
    }

    menuLeft = IMG_LoadTexture(renderer, "assets/ghostM.png");
    menuRight = IMG_LoadTexture(renderer, "assets/playerM.png");

    if (!menuLeft || !menuRight) {
        std::cout << "Failed to load menu textures: " << IMG_GetError() << std::endl;
    }

    backgroundTexture = IMG_LoadTexture(renderer, "assets/background.png");
    if (!backgroundTexture) 
    {
        std::cout << "Failed to load background: " << IMG_GetError() << std::endl;
    }

    player1 = new Player("assets/playerright.png", "assets/playerleft.png", 350, 540);

    rightEnemies.push_back(new Enemy("assets/ghostright.png", "assets/ghostleft.png", player1, 950, 400, 1.0f));
    rightEnemies.push_back(new Enemy("assets/ghostright.png", "assets/ghostleft.png", player1, 950, 350, 1.0f));

    leftEnemies.push_back(new Enemy("assets/ghostright.png", "assets/ghostleft.png", player1, -50, 300, 1.0f));
    leftEnemies.push_back(new Enemy("assets/ghostright.png", "assets/ghostleft.png", player1, -50, 450, 1.0f));

    map = new Map();
    IMG_Init(IMG_INIT_PNG);
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;

        case SDL_KEYDOWN:
            keys[event.key.keysym.scancode] = true;

            if (currentState == MENU) 
            {
                HandleMenuInput();
            }
            else if (currentState == QUIZ) 
            {
                HandleQuizInput();
            }
            else if (currentState == GAME_OVER) 
            {
                HandleGameOverInput();
            }
            else if (currentState == PLAYING) 
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_J)
                {
                    int dir = player1->getDirection();
                    int bulletX = player1->getX() + (dir == 1 ? 80 : 30);
                    int bulletY = player1->getY() + 50;

                    bullets.push_back(new Bullet(
                        "assets/bulletright.png",
                        "assets/bulletleft.png",
                        bulletX, bulletY, dir, 20.0f
                    ));
                }
            }
            break;

        case SDL_KEYUP:
            keys[event.key.keysym.scancode] = false;
            break;
        }
    }
}

void Game::HandleMenuInput()
{
    if (keys[SDL_SCANCODE_RETURN]) 
    {
        currentState = PLAYING;
        playerLives = 3;
    }
}

void Game::HandleQuizInput()
{
    if (keys[SDL_SCANCODE_1]) selectedAnswer = 0;
    if (keys[SDL_SCANCODE_2]) selectedAnswer = 1;
    if (keys[SDL_SCANCODE_3]) selectedAnswer = 2;
    if (keys[SDL_SCANCODE_4]) selectedAnswer = 3;

    if (keys[SDL_SCANCODE_RETURN]) 
    {
        CheckQuizAnswer();
    }
}

void Game::HandleGameOverInput()
{
    if (keys[SDL_SCANCODE_R]) 
    {
        RestartGame();
    }
    if (keys[SDL_SCANCODE_M]) 
    {
        currentState = MENU;
        RestartGame();
    }
}

bool Game::isKeyPressed(SDL_Scancode key)
{
    return keys[key];
}

void Game::TriggerQuiz()
{
    if (currentState == PLAYING) 
    {
        currentState = QUIZ;
    }
}

void Game::ResetEnemies()
{
    for (auto e : rightEnemies) 
    {
        e->RespawnAtEdge(950, 600);
    }
    for (auto e : leftEnemies) 
    {
        e->RespawnAtEdge(-200, 600);
    }
}

void Game::GenerateQuizQuestion()
{
    int type = rand() % 4;
    int num1 = rand() % 20 + 1;
    int num2 = rand() % 20 + 1;
    int correctAns = 0;

    switch (type) 
    {
    case 0: // Addition
        currentQuestion.question = std::to_string(num1) + " + " + std::to_string(num2) + " = ?";
        correctAns = num1 + num2;
        break;
    case 1: // Subtraction
        if (num1 < num2) std::swap(num1, num2);
        currentQuestion.question = std::to_string(num1) + " - " + std::to_string(num2) + " = ?";
        correctAns = num1 - num2;
        break;
    case 2: // Multiplication
        num1 = rand() % 10 + 1;
        num2 = rand() % 10 + 1;
        currentQuestion.question = std::to_string(num1) + " x " + std::to_string(num2) + " = ?";
        correctAns = num1 * num2;
        break;
    case 3: // Division
        num2 = rand() % 9 + 1;
        num1 = num2 * (rand() % 10 + 1);
        currentQuestion.question = std::to_string(num1) + " / " + std::to_string(num2) + " = ?";
        correctAns = num1 / num2;
        break;
    }

    currentQuestion.correctAnswer = rand() % 4;
    currentQuestion.answers[currentQuestion.correctAnswer] = std::to_string(correctAns);

    for (int i = 0; i < 4; i++) 
    {
        if (i != currentQuestion.correctAnswer) 
        {
            int wrongAns = correctAns + (rand() % 10 - 5);
            if (wrongAns == correctAns) wrongAns += 5;
            currentQuestion.answers[i] = std::to_string(wrongAns);
        }
    }

    quizTimer = QUIZ_TIME_LIMIT;
    selectedAnswer = 0;
}

void Game::CheckQuizAnswer()
{
    if (selectedAnswer == currentQuestion.correctAnswer) 
    {
        std::cout << "Correct!" << std::endl;
    }
    else 
    {
        std::cout << "Wrong! Lost a life." << std::endl;
        playerLives--;
        if (playerLives <= 0) 
        {
            currentState = GAME_OVER;
            return;
        }
    }

    ResetEnemies();
    currentState = PLAYING;
}

void Game::update()
{
    if (currentState == MENU || currentState == GAME_OVER) 
    {
        return;
    }

    if (currentState == QUIZ) 
    {
        UpdateQuiz(1.0f / 60.0f);
        return;
    }

    player1->Update();

    for (auto e : rightEnemies) e->Update();
    for (auto e : leftEnemies) e->Update();

    SDL_Rect playerRect = { player1->getX(), player1->getY(), 174, 174 };
    for (auto e : rightEnemies) {
        if (CheckCollision(playerRect, e->GetRect(), 40)) 
        {
            TriggerQuiz();
            GenerateQuizQuestion();
            return;
        }
    }
    for (auto e : leftEnemies) 
    {
        if (CheckCollision(playerRect, e->GetRect(), 40)) 
        {
            TriggerQuiz();
            GenerateQuizQuestion();
            return;
        }
    }

    // Bullet updates
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i]->Update();
        SDL_Rect bRect = bullets[i]->GetRect();

        for (auto e : rightEnemies)
        {
            if (CheckCollision(bRect, e->GetRect(), 30))
            {
                e->RespawnAtEdge(-200, 600);
                delete bullets[i];
                bullets.erase(bullets.begin() + i);
                i--;
                goto nextBullet;
            }
        }

        for (auto e : leftEnemies)
        {
            if (CheckCollision(bRect, e->GetRect(), 30))
            {
                e->RespawnAtEdge(1100, 600);
                delete bullets[i];
                bullets.erase(bullets.begin() + i);
                i--;
                goto nextBullet;
            }
        }

        if (i < bullets.size() && bullets[i]->IsOffScreen(900, 600))
        {
            delete bullets[i];
            bullets.erase(bullets.begin() + i);
            i--;
        }

    nextBullet:;
    }
}

void Game::UpdateQuiz(float deltaTime)
{
    quizTimer -= deltaTime;
    if (quizTimer <= 0) 
    {
        std::cout << "Time's up! Lost a life." << std::endl;
        playerLives--;
        if (playerLives <= 0) 
        {
            currentState = GAME_OVER;
        }
        else 
        {
            ResetEnemies();
            currentState = PLAYING;
        }
    }
}

void Game::DrawBox(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
}

void Game::DrawText(const std::string& text, int x, int y, int size, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    auto getFont = [](char c) -> const int* {
        static const int fontA[] = { 0x70, 0x88, 0xF8, 0x88, 0x88 };
        static const int fontB[] = { 0xF0, 0x88, 0xF0, 0x88, 0xF0 };
        static const int fontC[] = { 0x70, 0x80, 0x80, 0x80, 0x70 };
        static const int fontD[] = { 0xF0, 0x88, 0x88, 0x88, 0xF0 };
        static const int fontE[] = { 0xF8, 0x80, 0xF0, 0x80, 0xF8 };
        static const int fontF[] = { 0xF8, 0x80, 0xF0, 0x80, 0x80 };
        static const int fontG[] = { 0x70, 0x80, 0xB8, 0x88, 0x70 };
        static const int fontH[] = { 0x88, 0x88, 0xF8, 0x88, 0x88 };
        static const int fontI[] = { 0x70, 0x20, 0x20, 0x20, 0x70 };
        static const int fontJ[] = { 0x38, 0x10, 0x10, 0x90, 0x60 };
        static const int fontK[] = { 0x88, 0x90, 0xE0, 0x90, 0x88 };
        static const int fontL[] = { 0x80, 0x80, 0x80, 0x80, 0xF8 };
        static const int fontM[] = { 0x88, 0xD8, 0xA8, 0x88, 0x88 };
        static const int fontN[] = { 0x88, 0xC8, 0xA8, 0x98, 0x88 };
        static const int fontO[] = { 0x70, 0x88, 0x88, 0x88, 0x70 };
        static const int fontP[] = { 0xF0, 0x88, 0xF0, 0x80, 0x80 };
        static const int fontQ[] = { 0x70, 0x88, 0x88, 0xA8, 0x78 };
        static const int fontR[] = { 0xF0, 0x88, 0xF0, 0x90, 0x88 };
        static const int fontS[] = { 0x70, 0x80, 0x70, 0x08, 0x70 };
        static const int fontT[] = { 0xF8, 0x20, 0x20, 0x20, 0x20 };
        static const int fontU[] = { 0x88, 0x88, 0x88, 0x88, 0x70 };
        static const int fontV[] = { 0x88, 0x88, 0x88, 0x50, 0x20 };
        static const int fontW[] = { 0x88, 0x88, 0xA8, 0xD8, 0x88 };
        static const int fontX[] = { 0x88, 0x50, 0x20, 0x50, 0x88 };
        static const int fontY[] = { 0x88, 0x88, 0x50, 0x20, 0x20 };
        static const int fontZ[] = { 0xF8, 0x10, 0x20, 0x40, 0xF8 };
        static const int font0[] = { 0x70, 0x98, 0xA8, 0xC8, 0x70 };
        static const int font1[] = { 0x20, 0x60, 0x20, 0x20, 0x70 };
        static const int font2[] = { 0x70, 0x88, 0x10, 0x20, 0xF8 };
        static const int font3[] = { 0x70, 0x88, 0x30, 0x88, 0x70 };
        static const int font4[] = { 0x10, 0x30, 0x50, 0xF8, 0x10 };
        static const int font5[] = { 0xF8, 0x80, 0xF0, 0x08, 0xF0 };
        static const int font6[] = { 0x30, 0x40, 0xF0, 0x88, 0x70 };
        static const int font7[] = { 0xF8, 0x08, 0x10, 0x20, 0x20 };
        static const int font8[] = { 0x70, 0x88, 0x70, 0x88, 0x70 };
        static const int font9[] = { 0x70, 0x88, 0x78, 0x08, 0x70 };
        static const int fontPlus[] = { 0x00, 0x20, 0x70, 0x20, 0x00 };
        static const int fontMinus[] = { 0x00, 0x00, 0x70, 0x00, 0x00 };
        static const int fontMult[] = { 0x00, 0x50, 0x20, 0x50, 0x00 };
        static const int fontDiv[] = { 0x08, 0x10, 0x20, 0x40, 0x80 };
        static const int fontColon[] = { 0x00, 0x20, 0x00, 0x20, 0x00 };
        static const int fontEqual[] = { 0x00, 0x70, 0x00, 0x70, 0x00 };
        static const int fontQues[] = { 0x70, 0x88, 0x10, 0x00, 0x10 };
        static const int fontExcl[] = { 0x20, 0x20, 0x20, 0x00, 0x20 };
        static const int fontSpace[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

        switch (c) {
        case 'A': return fontA;
        case 'B': return fontB;
        case 'C': return fontC;
        case 'D': return fontD;
        case 'E': return fontE;
        case 'F': return fontF;
        case 'G': return fontG;
        case 'H': return fontH;
        case 'I': return fontI;
        case 'J': return fontJ;
        case 'K': return fontK;
        case 'L': return fontL;
        case 'M': return fontM;
        case 'N': return fontN;
        case 'O': return fontO;
        case 'P': return fontP;
        case 'Q': return fontQ;
        case 'R': return fontR;
        case 'S': return fontS;
        case 'T': return fontT;
        case 'U': return fontU;
        case 'V': return fontV;
        case 'W': return fontW;
        case 'X': return fontX;
        case 'Y': return fontY;
        case 'Z': return fontZ;
        case '0': return font0;
        case '1': return font1;
        case '2': return font2;
        case '3': return font3;
        case '4': return font4;
        case '5': return font5;
        case '6': return font6;
        case '7': return font7;
        case '8': return font8;
        case '9': return font9;
        case '+': return fontPlus;
        case '-': return fontMinus;
        case '*': case 'x': return fontMult;
        case '/': return fontDiv;
        case ':': return fontColon;
        case '=': return fontEqual;
        case '?': return fontQues;
        case '!': return fontExcl;
        case ' ': return fontSpace;
        default: return fontSpace;
        }
        };

    int cursorX = x;
    for (char c : text) 
    {
        char ch = (c >= 'a' && c <= 'z') ? c - 32 : c;

        if (ch == ' ') 
        {
            cursorX += size * 6;
            continue;
        }

        const int* pattern = getFont(ch);

        for (int row = 0; row < 5; row++) 
        {
            for (int col = 0; col < 8; col++) 
            {
                if (pattern[row] & (0x80 >> col)) 
                {
                    SDL_Rect pixel = {
                        cursorX + col * size,
                        y + row * size,
                        size,
                        size
                    };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }

        cursorX += size * 6;
    }

    SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
}

void Game::render()
{
    SDL_RenderClear(renderer);

    if (currentState == MENU) 
    {
        RenderMenu();
    }
    else if (currentState == GAME_OVER) 
    {
        RenderGameOver();
    }
    else 
    {

        map->DrawMap();
        SDL_Rect bgRect = { 0, 0, 1000, 800 };
        SDL_RenderCopy(Game::renderer, backgroundTexture, NULL, &bgRect);
        player1->Render();

        for (auto e : rightEnemies) e->Render();
        for (auto e : leftEnemies) e->Render();
        for (auto bullet : bullets) bullet->Render();

        DrawText("LIVES: " + std::to_string(playerLives), 10, 10, 3, 255, 255, 255);

        if (currentState == QUIZ) 
        {
            RenderQuiz();
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::RenderMenu()
{
    SDL_RenderClear(renderer);

    // Left sprite
    SDL_Rect leftRect;
    leftRect.x = 50; 
    leftRect.y = 150;
    leftRect.w = 450; 
    leftRect.h = 450;
    SDL_RenderCopy(renderer, menuLeft, NULL, &leftRect);

    // Right sprite
    SDL_Rect rightRect;
    rightRect.x = 550 ;
    rightRect.y = 150;
    rightRect.w = 450;
    rightRect.h = 450;
    SDL_RenderCopy(renderer, menuRight, NULL, &rightRect);

    SDL_RenderPresent(renderer);

    DrawText("MATH-O-Lantern", 250, 200, 5, 255, 255, 0);
    DrawText("PRESS ENTER TO START", 350, 400, 3, 255, 0, 0);
    DrawText("WASD - MOVE  SPACE - JUMP  J - SHOOT", 150, 600, 2, 200, 200, 200);
}

void Game::RenderQuiz()
{
    DrawBox(150, 100, 700, 500, 0, 0, 0, 220);

    int timerVal = (int)quizTimer;
    std::string timerText = "TIME: " + std::to_string(timerVal);
    if (quizTimer < 10)
    {
        DrawText(timerText, 400, 120, 4, 255, 50, 50);
    }
    else
    {
        DrawText(timerText, 400, 120, 4, 255, 255, 255);
    }

    DrawText(currentQuestion.question, 300, 200, 5, 255, 255, 0);

    const char answerLabels[] = { 'A', 'B', 'C', 'D' };

    for (int i = 0; i < 4; i++)
    {
        std::string ansText = std::string(1, answerLabels[i]) + ". " + currentQuestion.answers[i];
        if (i == selectedAnswer)
        {
            DrawBox(190, 290 + i * 60, 500, 45, 100, 100, 255, 150);
            DrawText(ansText, 200, 300 + i * 60, 4, 255, 255, 0);
        }
        else
        {
            DrawText(ansText, 200, 300 + i * 60, 4, 255, 255, 255);
        }
    }

    DrawText("PRESS 1-4 TO SELECT  ENTER TO CONFIRM", 180, 550, 2, 200, 200, 200);
}

void Game::RenderGameOver()
{
    DrawText("GAME OVER", 300, 250, 8, 255, 0, 0);
    DrawText("PRESS R TO RESTART", 280, 400, 4, 255, 255, 255);
}

void Game::RestartGame()
{
    playerLives = 3;
    currentState = PLAYING;

    player1->setX(350);
    player1->setY(540);

    for (auto bullet : bullets) delete bullet;
    bullets.clear();

    ResetEnemies();
}

void Game::clean()
{
    for (auto bullet : bullets) delete bullet;
    bullets.clear();

    for (auto e : rightEnemies) delete e;
    rightEnemies.clear();

    for (auto e : leftEnemies) delete e;
    leftEnemies.clear();

    delete player1;
    delete map;

    SDL_DestroyTexture(backgroundTexture);
    backgroundTexture = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}