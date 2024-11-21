#ifndef CLIENT_H
#define CLIENT_H

#include "purrsock/purrsock.h"
#include "../common/game_state.h"
#include "game.hpp"
#include "raylib.h"

typedef struct {
    ps_socket_t socket;
} Client;

typedef struct {
    const char* server_ip;
    ps_port_t server_port;
    ps_socket_t socket;
} Server;

bool InitClient(Client* client, Server* server, const char* server_ip, ps_port_t server_port);
bool SendGameState(Client* client, Server* server, const GameState& gameState);
bool ReceiveGameState(Client* client, Server* server, Paddle paddles[4], Ball* ball);
void CloseClient(Client* client, Server* server);

#endif // CLIENT_H
