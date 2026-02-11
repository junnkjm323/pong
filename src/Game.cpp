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
    // Create SDL contexts
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

    // Initialize paddles and balls
    mPaddlePos_p1 = {10.0f, 768.0f/2.0f};
    mPaddlePos_p2 = {1024.0f-10.0f, 768.0f/2.0f};

    // Add first ball
    mBall.emplace_back(
        Vector2({1024.0f/2.0f, 768.0f/2.0f}),
        Vector2({-200.0f, 235.0f})
    );

    return true;
}

// Run gameloop
void Game::RunLoop() {
    while (mIsRunning) {
        ProcessInput();     // Note: Inputs include input devices, communication data, or replay datas.
        UpdateGame();
        GenerateOutput();   // Note: Output include graphics, audios, or force feedback controllers.
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
        // Quit Game with [X] button upper-right of window
        switch (event.type)
        {
        case SDL_QUIT:
            mIsRunning = false;
        break;
        case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_B && event.key.repeat == 0) {
                // Add balls
                mBall.emplace_back(
                    Vector2({1024.0f/2.0f, 768.0f/2.0f}), 
                    Vector2({-200.0f, 235.0f})
                );
            }
        break; 
        default:
            break;
        }
    }

    // Keyboard Input
    const Uint8* state = SDL_GetKeyboardState(NULL);
    // Quit Game with ESC key
    if (state[SDL_SCANCODE_ESCAPE]) {
        mIsRunning = false;
    }

    // Paddle control
    // Player 1
    mPaddleDir_p1 = 0;
    // Move up with W key
    if (state[SDL_SCANCODE_W]) {
        mPaddleDir_p1 -= 1;
    }
    // Move down with S key
    if (state[SDL_SCANCODE_S]) {
        mPaddleDir_p1 += 1;
    }

    // Player 2
    mPaddleDir_p2 = 0;
    // Move up with I key
    if (state[SDL_SCANCODE_I]) {
        mPaddleDir_p2 -= 1;
    }
    // Move down with K key
    if (state[SDL_SCANCODE_K]) {
        mPaddleDir_p2 += 1;
    }
}

void Game::UpdateGame()
{
    // Get Deltatime
    // Note: Game logic must be updated by Deltatime.
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), mTickCount+16));
    float deltaTime = (SDL_GetTicks() - mTickCount) / 1000.0f;
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }
    mTickCount = SDL_GetTicks();

    // Paddle position update
    // Player1 paddle
    if (mPaddleDir_p1 != 0) {
        mPaddlePos_p1.y += mPaddleDir_p1 * paddle_speed * deltaTime;
        // Wall clamp
        if (mPaddlePos_p1.y < paddle_width/2.0f + wall_thickness) {
            mPaddlePos_p1.y = paddle_width/2.0f + wall_thickness;
        } else if (mPaddlePos_p1.y > 768.0f - paddle_width/2.0f - wall_thickness) {
            mPaddlePos_p1.y = 768.0f - paddle_width/2.0f - wall_thickness;
        }
    }
    // Player2 paddle
    if (mPaddleDir_p2 != 0) {
        mPaddlePos_p2.y += mPaddleDir_p2 * paddle_speed * deltaTime;
        // Wall clamp
        if (mPaddlePos_p2.y < paddle_width/2.0f + wall_thickness) {
            mPaddlePos_p2.y = paddle_width/2.0f + wall_thickness;
        } else if (mPaddlePos_p2.y > 768.0f - paddle_width/2.0f - wall_thickness) {
            mPaddlePos_p2.y = 768.0f - paddle_width/2.0f - wall_thickness;
        }
    }

    // Ball position update
    for (Ball& ball : mBall) {
        // Ball position update
        ball.Pos.x += ball.Vel.x * deltaTime;
        ball.Pos.y += ball.Vel.y * deltaTime;
        // Wall bounces
        // Note: Without velocity condition, the ball does not bounce when it hits the wall
        // Upper wall
        if (ball.Pos.y <= wall_thickness && ball.Vel.y < 0.0f) {
            ball.Vel.y *= -1;
        }
        // Lower wall
        if (ball.Pos.y >= 768.0f-wall_thickness && ball.Vel.y > 0.0f) {
            ball.Vel.y *= -1;
        }

        // Paddle bounce
        float diff = mPaddlePos_p1.y - ball.Pos.y;
        diff = (diff > 0.0f) ? diff : -diff;
        if (diff<=paddle_width/2.0f && ball.Pos.x<=25.0f && ball.Pos.x>=20.0f && ball.Vel.x<0.0f) {
            ball.Vel.x *= -1;
        }

        diff = mPaddlePos_p2.y - ball.Pos.y;
        diff = (diff > 0.0f) ? diff : -diff;
        if (diff<=paddle_width/2.0f && ball.Pos.x>=1024.0f-25.0f && ball.Pos.x<=1024.0f-20.0f && ball.Vel.x>0.0f) {
            ball.Vel.x *= -1;
        }
    }
}

void Game::GenerateOutput()
{
    // Clear background color
    SDL_SetRenderDrawColor (
        mRenderer,
        0,
        0,
        0,
        255
    );
    SDL_RenderClear(mRenderer);

    // Game rendering
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    // Wall rendering
    SDL_Rect wall {
        0,
        0,
        1024,
        wall_thickness
    };
    // Upper wall
    SDL_RenderFillRect(mRenderer, &wall);
    // Lower wall
    wall.y = 768 - wall_thickness;
    SDL_RenderFillRect(mRenderer, &wall);

    // Ball rendering
    for (Ball ball : mBall) {
        SDL_Rect ball_rect {
            static_cast<int>(ball.Pos.x - ball_size/2),
            static_cast<int>(ball.Pos.y - ball_size/2),
            ball_size,
            ball_size
        };
        SDL_RenderFillRect(mRenderer, &ball_rect);
    }

    // Paddle rendering
    SDL_Rect paddle_p1 {
        static_cast<int>(mPaddlePos_p1.x - paddle_thickness/2),
        static_cast<int>(mPaddlePos_p1.y - paddle_width/2),
        paddle_thickness,
        paddle_width,
    };
    SDL_Rect paddle_p2 {
        static_cast<int>(mPaddlePos_p2.x - paddle_thickness/2),
        static_cast<int>(mPaddlePos_p2.y - paddle_width/2),
        paddle_thickness,
        paddle_width,
    };
    SDL_RenderFillRect(mRenderer, &paddle_p1);
    SDL_RenderFillRect(mRenderer, &paddle_p2);

    SDL_RenderPresent(mRenderer);
}