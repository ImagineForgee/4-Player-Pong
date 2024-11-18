#include "raylib.h"
#include <math.h>
#include "game.hpp"
#include "settings.hpp"

void InitGame(Paddle paddles[4], Ball* ball, int scores[4]) {
    InitWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "4-Player Pong");
    SetTargetFPS(60);

    paddles[0] = { 0.5f, COLOR_TOP };
    paddles[1] = { 0.5f, COLOR_BOTTOM };
    paddles[2] = { 0.5f, COLOR_LEFT };
    paddles[3] = { 0.5f, COLOR_RIGHT };
    SpawnBall(ball);
    for (int i = 0; i < 4; i++) {
        scores[i] = 0;
    }
}

void DrawBall(Ball ball) {
    DrawCircleV(ball.position, BALL_SIZE, WHITE);
}

void DrawPaddle(Paddle paddle, int side) {
    switch (side) {
    case 0: // Top
        DrawRectangle(
            (int)(SCREEN_SIZE.x * paddle.position - PADDLE_SIZE / 2), 0,
            (int)PADDLE_SIZE, (int)PADDLE_THICKNESS, paddle.color);
        break;
    case 1: // Bottom
        DrawRectangle(
            (int)(SCREEN_SIZE.x * paddle.position - PADDLE_SIZE / 2), (int)(SCREEN_SIZE.y - PADDLE_THICKNESS),
            (int)PADDLE_SIZE, (int)PADDLE_THICKNESS, paddle.color);
        break;
    case 2: // Left
        DrawRectangle(
            0, (int)(SCREEN_SIZE.y * paddle.position - PADDLE_SIZE / 2),
            (int)PADDLE_THICKNESS, (int)PADDLE_SIZE, paddle.color);
        break;
    case 3: // Right
        DrawRectangle(
            (int)(SCREEN_SIZE.x - PADDLE_THICKNESS), (int)(SCREEN_SIZE.y * paddle.position - PADDLE_SIZE / 2),
            (int)PADDLE_THICKNESS, (int)PADDLE_SIZE, paddle.color);
        break;
    }
}

bool CheckPaddleCollision(Ball ball, Paddle paddle, int side) {
    switch (side) {
    case 0: // Top
        return (ball.position.y - BALL_SIZE <= PADDLE_THICKNESS &&
            ball.position.x >= SCREEN_SIZE.x * paddle.position - PADDLE_SIZE / 2 &&
            ball.position.x <= SCREEN_SIZE.x * paddle.position + PADDLE_SIZE / 2);
    case 1: // Bottom
        return (ball.position.y + BALL_SIZE >= SCREEN_SIZE.y - PADDLE_THICKNESS &&
            ball.position.x >= SCREEN_SIZE.x * paddle.position - PADDLE_SIZE / 2 &&
            ball.position.x <= SCREEN_SIZE.x * paddle.position + PADDLE_SIZE / 2);
    case 2: // Left
        return (ball.position.x - BALL_SIZE <= PADDLE_THICKNESS &&
            ball.position.y >= SCREEN_SIZE.y * paddle.position - PADDLE_SIZE / 2 &&
            ball.position.y <= SCREEN_SIZE.y * paddle.position + PADDLE_SIZE / 2);
    case 3: // Right
        return (ball.position.x + BALL_SIZE >= SCREEN_SIZE.x - PADDLE_THICKNESS &&
            ball.position.y >= SCREEN_SIZE.y * paddle.position - PADDLE_SIZE / 2 &&
            ball.position.y <= SCREEN_SIZE.y * paddle.position + PADDLE_SIZE / 2);
    }
    return false;
}

void SpawnBall(Ball* ball) {
    float angle = GetRandomValue(30, 150) * (PI / 180.0f);
    float velocityX = cos(angle) * BALL_SPEED_X;
    float velocityY = sin(angle) * BALL_SPEED_Y;
    velocityX *= (GetRandomValue(0, 1) == 0) ? -1 : 1;
    velocityY *= (GetRandomValue(0, 1) == 0) ? -1 : 1;
    ball->velocity = { velocityX, velocityY };
    ball->position = { SCREEN_SIZE.x / 2.0f, SCREEN_SIZE.y / 2.0f };
}

void ClampScores(int scores[4]) {
    for (int i = 0; i < 4; i++) {
        if (scores[i] < 0) {
            scores[i] = 0;
        }
        if (scores[i] > SCORE_LIMIT) {
            scores[i] = SCORE_LIMIT;
        }
    }
}

void CheckBallCollision(Ball* ball, int scores[4]) {
    if (ball->position.x - BALL_SIZE <= 0 || ball->position.x + BALL_SIZE >= SCREEN_SIZE.x ||
        ball->position.y - BALL_SIZE <= 0 || ball->position.y + BALL_SIZE >= SCREEN_SIZE.y) {

        if (ball->position.x - BALL_SIZE <= 0) {
            scores[3] += 1;
            SpawnBall(ball);
        }
        else if (ball->position.x + BALL_SIZE >= SCREEN_SIZE.x) {
            scores[2] += 1;
            SpawnBall(ball);
        }
        else if (ball->position.y - BALL_SIZE <= 0) {
            scores[1] += 1;
            SpawnBall(ball);
        }
        else if (ball->position.y + BALL_SIZE >= SCREEN_SIZE.y) {
            scores[0] += 1;
            SpawnBall(ball);
        }
        ClampScores(scores);
        
    }
}

void DrawScores(int scores[4]) {
    DrawText(TextFormat("Red: %d", scores[0]), (int)(SCREEN_SIZE.x / 2 - MeasureText(TextFormat("Top: %d", scores[0]), 20) / 2), 10, 20, COLOR_TOP);
    DrawText(TextFormat("Blue: %d", scores[1]), (int)(SCREEN_SIZE.x / 2 - MeasureText(TextFormat("Bottom: %d", scores[1]), 20) / 2), (int)(SCREEN_SIZE.y - 30), 20, COLOR_BOTTOM);
    DrawText(TextFormat("Green: %d", scores[2]), 10, (int)(SCREEN_SIZE.y / 2 - 10), 20, COLOR_LEFT);
    DrawText(TextFormat("Yellow: %d", scores[3]), (int)(SCREEN_SIZE.x - 100), (int)(SCREEN_SIZE.y / 2 - 10), 20, COLOR_RIGHT);
}
