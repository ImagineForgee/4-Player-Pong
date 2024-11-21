#include "client.hpp"
#include <iostream>
#include <cstring>
#include "../common/game_state.h"

bool CheckResult(ps_result_t result, const char* operation) {
    if (result != PS_SUCCESS) {
        std::cerr << operation << " failed: " << ps_result_to_cstr(result) << std::endl;
        return false;
    }
    return true;
}

bool InitClient(Client* client, Server* server, const char* server_ip, ps_port_t server_port) {
    if (ps_init() != true) {
        return false;
    }

    ps_result_t cresult = ps_create_socket(&client->socket, PS_PROTOCOL_UDP);
    if (!CheckResult(cresult, "Create UDP socket")) {
        return false;
    }

    ps_result_t bind_result = ps_bind_socket(client->socket, nullptr, 0);
    if (!CheckResult(bind_result, "Bind client socket")) {
        return false;
    }

    server->server_ip = server_ip;
    server->server_port = server_port;

    ps_result_t sresult = ps_create_socket_from_addr(&server->socket, PS_PROTOCOL_UDP, server->server_ip, server->server_port);
    if (!CheckResult(sresult, "Create UDP socket from server address")) {
        return false;
    }

    std::cout << "UDP socket created. Ready to send/receive packets.\n";
    return true;
}

bool SendGameState(Client* client, Server* server, const GameState& gameState) {
    ps_packet_t packet;
    packet.size = sizeof(gameState);
    packet.buf = const_cast<char*>(reinterpret_cast<const char*>(&gameState));

    ps_result_t result = ps_send_socket_packet(client->socket, packet, server->socket);
    if (!CheckResult(result, "Send game state to server")) {
        return false;
    }

    std::cout << "Game state sent successfully.\n";
    return true;
}

bool ReceiveGameState(Client* client, Server* server, Paddle paddles[4], Ball* ball) {
    ps_packet_t packet;
    GameState gamestate;

    initializeGameState(gamestate);

    packet.size = sizeof(gamestate);
    packet.buf = reinterpret_cast<char*>(&gamestate);

    std::cout << "Receiving packet..." << std::endl;
    std::cout << "Packet buffer size: " << packet.size << std::endl;
    std::cout << "Packet buffer address: " << static_cast<void*>(packet.buf) << std::endl;

    ps_result_t result = ps_read_socket_packet(client->socket, &packet, &server->socket);
    std::cout << "Read result: " << ps_result_to_cstr(result) << std::endl;

    if (!CheckResult(result, "Read socket packet")) {
        return false;
    }

    std::cout << "Received packet of size " << packet.size << std::endl;

    for (int i = 0; i < 4; ++i) {
        paddles[i].position = gamestate.paddlePositions[i];
    }
    ball->position.x = gamestate.ballPosition[0];
    ball->position.y = gamestate.ballPosition[1];

    return true;
}

void CloseClient(Client* client, Server* server) {
    ps_destroy_socket(client->socket);
    ps_destroy_socket(server->socket);
    std::cout << "Client socket destroyed successfully.\n";
}
