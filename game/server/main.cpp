#include "purrsock/purrsock.h"
#include "../common/game_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>

#define SERVER_PORT 12345
#define MAX_CLIENTS 4

#define PS_SOCKET_INVALID ((ps_socket_t)-1)

typedef struct {
    ps_socket_t socket;
    int id;
} Client;


Client clients[MAX_CLIENTS];
int client_count = 0;
std::mutex client_lock;

GameState gameState;

void handle_client(Client* client) {
    ps_packet_t packet;
    packet.buf = (char*)malloc(1024);
    packet.capacity = 1024;

    while (true) {
        ps_result_t result = ps_read_socket_packet(client->socket, &packet, NULL);
        if (result == PS_ERROR_CONNRESET || result == PS_ERROR_CONNREFUSED || result == PS_CONNCLOSED) {
            printf("Client %d disconnected.\n", client->id);
            break;
        }
        else if (result == PS_SUCCESS) {
            std::memcpy(gameState.paddlePositions, packet.buf, sizeof(float) * 4);

            gameState.ballPosition[0] += 0.01f; 
            gameState.ballPosition[1] += 0.01f;

            std::lock_guard<std::mutex> lock(client_lock);

            for (int i = 0; i < client_count; ++i) {
                if (clients[i].socket != PS_SOCKET_INVALID && clients[i].id != client->id) {
                    ps_packet_t gameStatePacket;
                    gameStatePacket.size = sizeof(GameState);
                    gameStatePacket.buf = (char*)&gameState;

                    ps_send_socket_packet(clients[i].socket, gameStatePacket, client->socket);
                    free(gameStatePacket.buf);
                }
            }
        }
        else {
            printf("Error reading packet from client %d: %d\n", client->id, result);
        }
    }

    free(packet.buf);
    ps_destroy_socket(client->socket);

    std::lock_guard<std::mutex> lock(client_lock);
    clients[client->id].socket = PS_SOCKET_INVALID;
    client_count--;
}


int main() {
    if (!ps_init()) {
        printf("Failed to initialize networking.\n");
        return -1;
    }

    ps_socket_t server_socket;
    if (ps_create_socket(&server_socket, PS_PROTOCOL_TCP) != PS_SUCCESS) {
        printf("Failed to create server socket.\n");
        return -1;
    }

    if (ps_bind_socket(server_socket, "0.0.0.0", SERVER_PORT) != PS_SUCCESS) {
        printf("Failed to bind server socket.\n");
        ps_destroy_socket(server_socket);
        return -1;
    }

    if (ps_listen_socket(server_socket) != PS_SUCCESS) {
        printf("Failed to listen on server socket.\n");
        ps_destroy_socket(server_socket);
        return -1;
    }

    printf("Server is listening on port %d...\n", SERVER_PORT);

    while (true) {
        ps_socket_t client_socket;

        if (ps_accept_socket(server_socket, &client_socket) == PS_SUCCESS) {
            std::lock_guard<std::mutex> lock(client_lock);
            if (client_count >= MAX_CLIENTS) {
                printf("Maximum client limit reached, rejecting connection.\n");
                ps_destroy_socket(client_socket);
                continue;
            }
            Client new_client = { client_socket, client_count };
            clients[client_count++] = new_client;

            printf("Client %d connected.\n", new_client.id);

            std::thread client_thread(handle_client, &new_client);
            client_thread.detach();
        }
        else {
            printf("Failed to accept client connection.\n");
        }
    }
    ps_cleanup();
    return 0;
}
