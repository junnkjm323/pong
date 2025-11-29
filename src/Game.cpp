#include <SDL2/SDL.h>
#include "Game.h"

const int wall_thickness = 15;
const int ball_size = 15;
const int paddle_thickness = 15;
const int paddle_width = 100;
const float paddle_speed = 300.0f;

// Publics
Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mIsRunning(true)
,mTickCount(0) {

}

bool Game::Initialize() {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0) {
        SDL_Log("SDL Initialize failed: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
        "Pong",
        100, 100,
        1024, 768,
        0
    );
    if (!mWindow) {
        SDL_Log("SDL failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer (
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!mRenderer) {
        SDL_Log("SDL failed to create renderer: %s", SDL_GetError());
        return false;
    }
    mPaddlePos = {10.0f, 768.0f/2.0f};
    mBallPos = {1024.0f/2.0f, 768.0f/2.0f};
    mBallVel = {-200.0f, 235.0f};

    return true;
}

void Game::RunLoop() {
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown() {
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
    SDL_Quit();
}

// Privates
void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Quit Game with X
        switch (event.type)
        {
        case SDL_QUIT:
            mIsRunning = false;
            break;
        
        default:
            break;
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        // Quit Game with ESC key
        if (state[SDL_SCANCODE_ESCAPE]) {
            mIsRunning = false;
        }

        mPaddleDir = 0;
        if (state[SDL_SCANCODE_W]) {
            mPaddleDir -= 1;
        }
        if (state[SDL_SCANCODE_S]) {
            mPaddleDir += 1;
        }
    }
}

void Game::UpdateGame()
{
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), mTickCount+16));
    float deltaTime = (SDL_GetTicks() - mTickCount) / 1000.0f;
    
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    mTickCount = SDL_GetTicks();

    // Paddle position update
    if (mPaddleDir != 0) {
        mPaddlePos.y += mPaddleDir * paddle_speed * deltaTime;
        // Wall clamp
        if (mPaddlePos.y < paddle_width/2.0f + wall_thickness) {
            mPaddlePos.y = paddle_width/2.0f + wall_thickness;
        } else if (mPaddlePos.y > 768.0f - paddle_width/2.0f - wall_thickness) {
            mPaddlePos.y = 768.0f - paddle_width/2.0f - wall_thickness;
        }
    }

    // Ball position update
    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;
    // Wall bounce
    if (mBallPos.y <= wall_thickness && mBallVel.y < 0.0f) {
        mBallVel.y *= -1;
    }
    if (mBallPos.y >= 768.0f-wall_thickness && mBallVel.y > 0.0f) {
        mBallVel.y *= -1;
    }
    if (mBallPos.x >= 1024.0f-wall_thickness && mBallVel.x > 0.0f) {
        mBallVel.x *= -1;
    }

    float diff = mPaddlePos.y - mBallPos.y;
    diff = (diff > 0.0f) ? diff : -diff;
    if (diff<=paddle_width/2.0f && mBallPos.x<=25.0f && mBallPos.x>=20.0f && mBallVel.x<0.0f) {
        mBallVel.x *= -1;
    }
}

void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor (
        mRenderer,
        0,
        0,
        0,
        255
    );
    SDL_RenderClear(mRenderer);
    // Game Rendering

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_Rect wall {
        0,
        0,
        1024,
        wall_thickness
    };
    SDL_RenderFillRect(mRenderer, &wall);
    wall.y = 768 - wall_thickness;
    SDL_RenderFillRect(mRenderer, &wall);
    wall.x = 1024 - wall_thickness;
    wall.y = 0;
    wall.w = wall_thickness;
    wall.h = 768;
    SDL_RenderFillRect(mRenderer, &wall);

    SDL_Rect ball {
        static_cast<int>(mBallPos.x - ball_size/2),
        static_cast<int>(mBallPos.y - ball_size/2),
        ball_size,
        ball_size
    };
    SDL_RenderFillRect(mRenderer, &ball);

    SDL_Rect paddle {
        static_cast<int>(mPaddlePos.x - paddle_thickness/2),
        static_cast<int>(mPaddlePos.y - paddle_width/2),
        paddle_thickness,
        paddle_width,
    };
    SDL_RenderFillRect(mRenderer, &paddle);

    SDL_RenderPresent(mRenderer);
}