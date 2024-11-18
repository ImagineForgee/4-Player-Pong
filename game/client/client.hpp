#ifndef CLIENT_H
#define CLIENT_H

#include "purrsock/purrsock.h"
#include "game.hpp"
#include "raylib.h"

typedef struct {
    ps_socket_t socket;
    const char* server_ip;
    ps_port_t server_port;
} Client;

bool InitClient(Client* client, const char* server_ip, ps_port_t server_port);
bool SendPaddleMovement(Client* client, const float* paddlePositions, size_t data_size);
bool ReceiveGameState(Client* client, Paddle paddles[4], Ball* ball);
void CloseClient(Client* client);

#endif // CLIENT_H
