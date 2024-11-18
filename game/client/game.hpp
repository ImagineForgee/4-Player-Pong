#ifndef GAME_H
#define GAME_H

#include "raylib.h"


typedef struct {
    float position;
    Color color;
} Paddle;

typedef struct {
    Vector2 position;
    Vector2 velocity;
} Ball;

void InitGame(Paddle paddles[4], Ball* ball, int scores[4]);

void DrawPaddle(Paddle paddle, int side);
bool CheckPaddleCollision(Ball ball, Paddle paddle, int side);
void SpawnBall(Ball* ball);
void ClampScores(int scores[4]);
void CheckBallCollision(Ball* ball, int scores[4]);
void DrawScores(int scores[4]);
void DrawBall(Ball ball);
#endif // GAME_H
