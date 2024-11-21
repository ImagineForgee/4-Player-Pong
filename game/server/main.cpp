#include "purrsock/purrsock.h"
#include "../common/game_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVER_PORT 12345
#define MAX_CLIENTS 4

#define PS_SOCKET_INVALID ((ps_socket_t)-1)

typedef struct {
    ps_socket_t socket;
    ps_port_t port;
    const char* ip;
    int id;
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
std::mutex client_lock;

GameState gameState;

void handle_client(int client_id) {
    Client* client = &clients[client_id];
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
            printf("Received packet from client %d.\n", client->id);

            std::memcpy(gameState.paddlePositions, packet.buf, sizeof(float) * 4);
            gameState.ballPosition[0] += 0.01f;
            gameState.ballPosition[1] += 0.01f;

            std::lock_guard<std::mutex> lock(client_lock);
            for (int i = 0; i < client_count; ++i) {
                if (clients[i].socket != PS_SOCKET_INVALID && clients[i].id != client->id) {
                    ps_packet_t gameStatePacket;
                    gameStatePacket.size = sizeof(GameState);
                    gameStatePacket.buf = (char*)&gameState;

                    printf("GameState: ballPosition=(%.2f, %.2f), paddlePositions=(%.2f, %.2f, %.2f, %.2f), scores=(%d, %d, %d, %d)\n",
                        gameState.ballPosition[0], gameState.ballPosition[1],
                        gameState.paddlePositions[0], gameState.paddlePositions[1],
                        gameState.paddlePositions[2], gameState.paddlePositions[3],
                        gameState.scores[0], gameState.scores[1], gameState.scores[2], gameState.scores[3]);

                    ps_result_t send_result = ps_send_socket_packet(clients[i].socket, gameStatePacket, clients[i].socket);
                    if (send_result != PS_SUCCESS) {
                        printf("Failed to send game state to client %d. Result: %d\n", clients[i].id, send_result);
                    }
                    else {
                        printf("Game state sent to client %d.\n", clients[i].id);
                    }
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
    if (ps_create_socket(&server_socket, PS_PROTOCOL_UDP) != PS_SUCCESS) {
        printf("Failed to create server socket.\n");
        return -1;
    }

    if (ps_bind_socket(server_socket, "0.0.0.0", SERVER_PORT) != PS_SUCCESS) {
        printf("Failed to bind server socket.\n");
        ps_destroy_socket(server_socket);
        return -1;
    }

    printf("Server is listening on port %d...\n", SERVER_PORT);

    while (true) {
        initializeGameState(gameState);

        ps_packet_t packet;
        packet.buf = (char*)malloc(1024);
        packet.capacity = 1024;

        sockaddr_in client_address;
        socklen_t addr_len = sizeof(client_address);
        if (ps_create_socket_from_addr(Client->, PS_PROTOCOL_UDP, addr_len, ) != PS_SUCCESS) {
            printf("Failed to create server socket.\n");
            return -1;
        }

        ps_result_t result = ps_read_socket_packet(server_socket, &packet, (ps_socket_t*)&client_address);
        if (result == PS_SUCCESS) {
            bool client_found = false;
            for (int i = 0; i < client_count; ++i) {
                if (memcmp(&clients[i].address, &client_address, sizeof(client_address)) == 0) {
                    client_found = true;
                    break;
                }
            }

            if (!client_found && client_count < MAX_CLIENTS) {
                std::lock_guard<std::mutex> lock(client_lock);
                if (client_count >= MAX_CLIENTS) {
                    printf("Maximum client limit reached, ignoring new connection.\n");
                    continue;
                }

                clients[client_count].ip = client_address;
                clients[client_count].id = client_count;

                printf("New client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

                std::thread client_thread(handle_client, client_count);
                client_thread.detach();

                client_count++;
            }

            free(packet.buf);
        }
        else {
            printf("Error reading packet: %d\n", result);
        }
    }

    ps_cleanup();
    return 0;
}
