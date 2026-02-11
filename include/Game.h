#include <iostream>
#include <vector> 
#include <SDL2/SDL.h>

struct Vector2 {
    float x;
    float y;
};

struct Ball {
    Vector2 Pos;
    Vector2 Vel;

    // Constructor
    Ball(Vector2 pos, Vector2 vel) : Pos(pos), Vel(vel) {}
};

class Game {
public:
    Game();
    bool Initialize();
    void RunLoop();
    void Shutdown();
private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    bool mIsRunning;
    Uint32 mTickCount;

    // Player 1
    int mPaddleDir_p1;
    Vector2 mPaddlePos_p1;

    // Player 2
    int mPaddleDir_p2;
    Vector2 mPaddlePos_p2;

    std::vector<Ball> mBall;
};