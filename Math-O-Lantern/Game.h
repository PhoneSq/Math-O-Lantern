#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <string>

enum GameState {
    MENU,
    PLAYING,
    QUIZ,
    GAME_OVER
};

struct QuizQuestion 
{
    std::string question;
    std::string answers[4];
    int correctAnswer;
};

class Game
{
public:
    Game();
    ~Game();
    void init(const char* title, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() { return isRunning; }

    static SDL_Renderer* renderer;
    static bool keys[SDL_NUM_SCANCODES];
    static bool isKeyPressed(SDL_Scancode key);

    static GameState currentState;
    static int playerLives;
    static void TriggerQuiz();
    static void ResetEnemies();

private:
    bool isRunning = false;
    SDL_Window* window;

    QuizQuestion currentQuestion;
    int selectedAnswer;
    float quizTimer;
    const float QUIZ_TIME_LIMIT = 10.0f;

    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* menuLeft = nullptr;
    SDL_Texture* menuRight = nullptr;

    void GenerateQuizQuestion();
    void HandleQuizInput();
    void UpdateQuiz(float deltaTime);
    void RenderQuiz();
    void CheckQuizAnswer();

    void RenderMenu();
    void HandleMenuInput();

    void RenderGameOver();
    void HandleGameOverInput();

    void RestartGame();

    void DrawText(const std::string& text, int x, int y, int size, Uint8 r, Uint8 g, Uint8 b);
    void DrawBox(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
};
