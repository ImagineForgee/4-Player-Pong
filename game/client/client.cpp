#include "client.hpp"
#include <iostream>
#include <cstring>
#include "../common/game_state.h"

bool InitClient(Client* client, const char* server_ip, ps_port_t server_port) {
    if (ps_init() != true) {
        return false;
    }

    client->server_ip = server_ip;
    client->server_port = server_port;

    ps_result_t result = ps_create_socket(&client->socket, PS_PROTOCOL_TCP);
    if (result != PS_SUCCESS) {
        std::cerr << "Failed to create socket: " << ps_result_to_cstr(result) << std::endl;
        return false;
    }

    result = ps_connect_socket(client->socket, server_ip, server_port);
    if (result != PS_SUCCESS) {
        std::cerr << "Failed to connect to server: " << ps_result_to_cstr(result) << std::endl;
        ps_destroy_socket(client->socket);
        return false;
    }

    std::cout << "Connected to server at " << server_ip << ":" << server_port << std::endl;
    return true;
}

bool SendPaddleMovement(Client* client, const float* paddlePositions, size_t data_size) {
    ps_packet_t packet;
    packet.size = data_size;
    packet.buf = new char[data_size];
    std::memcpy(packet.buf, paddlePositions, data_size);

    ps_result_t result = ps_send_socket_packet(client->socket, packet, client->socket);
    delete[] packet.buf;

    return (result == PS_SUCCESS);
}

bool ReceiveGameState(Client* client, Paddle paddles[4], Ball* ball) {
    ps_packet_t packet;
    packet.size = sizeof(GameState);
    packet.buf = new char[packet.size];

    ps_result_t result = ps_read_socket_packet(client->socket, &packet, nullptr);
    if (result != PS_SUCCESS) {
        printf("Failed to receive packet. Result: %d\n", result);
        delete[] packet.buf;
        return false;
    }

    printf("Received packet of size %zu\n", packet.size);

    GameState* gameState = reinterpret_cast<GameState*>(packet.buf);

    for (int i = 0; i < 4; ++i) {
        paddles[i].position = gameState->paddlePositions[i];
    }
    ball->position.x = gameState->ballPosition[0];
    ball->position.y = gameState->ballPosition[1];

    delete[] packet.buf;
    return true;
}

void CloseClient(Client* client) {
    ps_destroy_socket(client->socket);
}
