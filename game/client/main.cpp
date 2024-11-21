#include "raylib.h"
#include "settings.hpp"
#include "game.hpp"
#include "client.hpp"
#include "cstdio";
#include "../common/game_state.h"

int main() {
    Client client;
    Server server;

    const char* server_ip = "127.0.0.1";
    ps_port_t server_port = 12345;

    if (!InitClient(&client, &server, server_ip, server_port)) {
        return -1;
    }

    Paddle paddles[4];
    Ball ball;
    int scores[4];
    GameState gameState;

    InitGame(paddles, &ball, scores);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT)) paddles[0].position -= 0.01f;
        if (IsKeyDown(KEY_RIGHT)) paddles[0].position += 0.01f;

        if (IsKeyDown(KEY_A)) paddles[1].position -= 0.01f;
        if (IsKeyDown(KEY_D)) paddles[1].position += 0.01;

        if (!ReceiveGameState(&client, &server, paddles, &ball)) {
            printf("Failed to receive game state\n");
        }

        for (int i = 0; i < 4; i++) {
            if (paddles[i].position < PADDLE_SIZE / 2.0f / SCREEN_SIZE.x) {
                gameState.paddlePositions[i];
                paddles[i].position = PADDLE_SIZE / 2.0f / SCREEN_SIZE.x;
            }
            if (paddles[i].position > 1.0f - PADDLE_SIZE / 2.0f / SCREEN_SIZE.x) {
                gameState.paddlePositions[i];
                paddles[i].position = 1.0f - PADDLE_SIZE / 2.0f / SCREEN_SIZE.x;
            }
        }
    

        ball.position.x += ball.velocity.x;
        ball.position.y += ball.velocity.y;
        
        for (int i = 0; i < 4; i++) {
            if (CheckPaddleCollision(ball, paddles[i], i)) {
                if (i == 0 || i == 1) {
                    ball.velocity.y *= -1;
                }
                else {
                    ball.velocity.x *= -1;
                }

                ball.velocity.x += (GetRandomValue(-10, 10) / 100.0f);
                ball.position.y += (GetRandomValue(-10, 10) / 100.0f);
            }
        }

        CheckBallCollision(&ball, scores);
        gameState.ballPosition[0] = ball.position.x;
        gameState.ballPosition[1] = ball.position.y;

        if (!SendGameState(&client, &server, gameState)) {
            printf("Failed to send gamestate data\n");
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < 4; i++) {
            DrawPaddle(paddles[i], i);
        }
        DrawBall(ball);
        DrawScores(scores);

        EndDrawing();
    }

    CloseClient(&client, &server);
    CloseWindow();
    return 0;
}
