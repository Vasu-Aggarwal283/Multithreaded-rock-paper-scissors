#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mutex>
#include <chrono>
#include <thread>
#include "gameLogic.h"

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

std::mutex gameMutex;

struct ClientInfo {
    int socket;
    int player;
};

std::vector<ClientInfo> clients;

void sendToPlayer(int playerNumber, const std::string& message) {
    for (const auto& client : clients) {
        if (client.player == playerNumber) {
            send(client.socket, message.c_str(), message.size(), 0);
            break;
        }
    }
}

void notifyOpponent(int currentPlayer, const std::string& move) {
    int opponentNumber = (currentPlayer == 1) ? 2 : 1;
    std::string notification = "Opponent played, Make your move!";
    sendToPlayer(opponentNumber, notification);
}

void sendToAllClients(const std::string& message) {
    for (const auto& client : clients) {
        send(client.socket, message.c_str(), message.size(), 0);
    }
}

void handleClient(int clientSocket, Game* game, int player) {
    char buffer[BUFFER_SIZE];

    // Notify both players that they can start making their moves
    std::string startMsg = "Make your move!";
    sendToPlayer(1, startMsg);
    sendToPlayer(2, startMsg);

    while (true) {
        std::memset(buffer, 0, BUFFER_SIZE);

        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            std::cout << "Player " << player << " disconnected.\n";
            std::string disconnectMsg = "Player " + std::to_string(player) + " disconnected. Game ended.";
            sendToAllClients(disconnectMsg);
            close(clientSocket);
            break;
        }

        std::string playerMove(buffer);
        
        {
            std::lock_guard<std::mutex> lock(gameMutex);
            
            // Register the move
            game->play(player, playerMove);

            // Notify opponent about the move
            notifyOpponent(player, playerMove);

            if (game->bothGone()) {
                // Determine and send the result
                int winner = game->findWinner();
                std::string result;
                if (winner == -1) {
                    result = "Draw!";
                } else if (winner == 0) {
                    result = "Player 1 wins!";
                } else {
                    result = "Player 2 wins!";
                }

                std::cout << "Game ended: " << result << std::endl;
                sendToAllClients(result);

                // Wait for 2 seconds before resetting the game
                std::this_thread::sleep_for(std::chrono::seconds(2));

                // Notify players to make their moves again
                std::string newRoundMsg = "Make your move!";
                sendToPlayer(1, newRoundMsg);
                sendToPlayer(2, newRoundMsg);

                // Reset the game state for a new round
                game->resetGame();
            }
        }
    }
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Failed to set socket options.\n";
        close(serverSocket);
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind to port " << PORT << ". Error: " << strerror(errno) << std::endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 2) == -1) {
        std::cerr << "Failed to listen.\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(clientAddr);

    Game game(1);

    // Accept first client
    int clientSocket1 = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
    std::cout << "Player 1 connected: " << clientSocket1 << std::endl;
    send(clientSocket1, "1", 1, 0);
    clients.push_back({clientSocket1, 1});

    // Accept second client
    int clientSocket2 = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
    std::cout << "Player 2 connected: " << clientSocket2 << std::endl;
    send(clientSocket2, "2", 1, 0);
    clients.push_back({clientSocket2, 2});

    std::cout << "Both players connected. Starting game...\n";
    game.setReady();

    std::thread t1(handleClient, clientSocket1, &game, 1);
    std::thread t2(handleClient, clientSocket2, &game, 2);

    t1.join();
    t2.join();

    close(serverSocket);
    return 0;
}
