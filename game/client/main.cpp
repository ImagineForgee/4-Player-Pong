#include "raylib.h"
#include "settings.hpp"
#include "game.hpp"
#include "client.hpp"
#include "cstdio";

int main() {
    Client client;
    const char* server_ip = "127.0.0.1";
    ps_port_t server_port = 12345;

    if (!InitClient(&client, server_ip, server_port)) {
        return -1;
    }

    Paddle paddles[4];
    Ball ball;
    int scores[4];

    InitGame(paddles, &ball, scores);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT)) paddles[0].position -= 0.01f;
        if (IsKeyDown(KEY_RIGHT)) paddles[0].position += 0.01f;

        if (IsKeyDown(KEY_A)) paddles[1].position -= 0.01f;
        if (IsKeyDown(KEY_D)) paddles[1].position += 0.01;

        for (int i = 0; i < 4; i++) {
            if (paddles[i].position < PADDLE_SIZE / 2.0f / SCREEN_SIZE.x) {
                paddles[i].position = PADDLE_SIZE / 2.0f / SCREEN_SIZE.x;
            }
            if (paddles[i].position > 1.0f - PADDLE_SIZE / 2.0f / SCREEN_SIZE.x) {
                paddles[i].position = 1.0f - PADDLE_SIZE / 2.0f / SCREEN_SIZE.x;
            }
        }

        if (!SendPaddleMovement(&client, (float*)paddles, sizeof(paddles))) {
            printf("Failed to send paddle movement data\n");
        }

        if (!ReceiveGameState(&client, paddles, &ball)) {
            printf("Failed to receive game state\n");
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

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < 4; i++) {
            DrawPaddle(paddles[i], i);
        }
        DrawBall(ball);
        DrawScores(scores);

        EndDrawing();
    }

    CloseClient(&client);
    CloseWindow();
    return 0;
}
