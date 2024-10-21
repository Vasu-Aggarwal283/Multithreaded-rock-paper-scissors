#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <thread>
#include <mutex>

class Network {
public:
    Network() {
        std::cout << "Network constructor started" << std::endl;
        serverIP = "127.0.0.1";
        serverPort = 12345;
        
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "Error creating socket." << std::endl;
            exit(1);
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);

        if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
            std::cerr << "Invalid address/Address not supported." << std::endl;
            exit(1);
        }
        
        if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Connection Failed." << std::endl;
            exit(1);
        }

        char buffer[1024] = {0};
        int valread = read(sockfd, buffer, 1024);
        if (valread < 0) {
            std::cerr << "Failed to read from server." << std::endl;
            exit(1);
        }
        playerID = atoi(buffer);
        std::cout << "You are Player " << playerID << std::endl;
    }

    std::string sendMove(const std::string &move) {
        send(sockfd, move.c_str(), move.size(), 0);
        return "Your move: " + move + "\nWaiting for opponent...";
    }

    std::string receiveMessage() {
        char buffer[1024] = {0};
        int bytesReceived = read(sockfd, buffer, 1024);
        if (bytesReceived < 0) {
            return "Error receiving message from server";
        }
        return std::string(buffer);
    }

    int getPlayerID() { return playerID; }
    int getSocket() { return sockfd; }

private:
    int sockfd;
    struct sockaddr_in serverAddr;
    std::string serverIP;
    int serverPort;
    int playerID;
};

class Button {
public:
    Button(std::string t, sf::Vector2f size, int charSize, sf::Color bgColor, sf::Color textColor) {
        text.setString(t);
        text.setFillColor(textColor);
        text.setCharacterSize(charSize);

        button.setSize(size);
        button.setFillColor(bgColor);
        originalColor = bgColor;
    }

    void setFont(sf::Font &font) {
        text.setFont(font);
    }

    void setPosition(sf::Vector2f pos) {
        button.setPosition(pos);
        float xPos = (pos.x + button.getGlobalBounds().width / 2) - (text.getGlobalBounds().width / 2);
        float yPos = (pos.y + button.getGlobalBounds().height / 2) - (text.getGlobalBounds().height / 2);
        text.setPosition(xPos, yPos);
    }

    void setEnabled(bool enabled) {
        if (enabled) {
            button.setFillColor(originalColor);
        } else {
            button.setFillColor(sf::Color(128, 128, 128));
        }
        isEnabled = enabled;
    }

    void drawTo(sf::RenderWindow &window) {
        window.draw(button);
        window.draw(text);
    }

    bool isMouseOver(sf::RenderWindow &window) {
        if (!isEnabled) return false;
        
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect btnPos = button.getGlobalBounds();
        return btnPos.contains(static_cast<sf::Vector2f>(mousePos));
    }

private:
    sf::RectangleShape button;
    sf::Text text;
    sf::Color originalColor;
    bool isEnabled = true;
};

std::mutex mtx;
std::string gameState = "Make your move!";  // Changed initial message
bool buttonsEnabled = true;

void receiveServerMessages(Network& network) {
    while (true) {
        std::string message = network.receiveMessage();
        if (message.empty()) continue;

        std::lock_guard<std::mutex> lock(mtx);

        // Update game state based on server message
        if (message.find("wins") != std::string::npos || message.find("Draw!") != std::string::npos) {
            gameState = message;  // Show the result
            buttonsEnabled = false;  // Disable buttons after game result
            std::this_thread::sleep_for(std::chrono::seconds(2));  // Wait for 2 seconds before resetting
            gameState = "Make your move!";  // Reset to initial message
            buttonsEnabled = true;  // Re-enable buttons for new round
        } else if (message.find("Waiting for") == std::string::npos) {
            // Only update the state for valid game state messages
            gameState = message;  // Avoid showing opponent's move
        }

        // Disable buttons after a move is made
        if (message.find("Your move:") != std::string::npos) {
            buttonsEnabled = false;  // Disable buttons after making a move
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(600, 400), "Rock Paper Scissors Client");
    
    sf::Font font;
    if (!font.loadFromFile("fonts/DejaVuSans.ttf")) {
        std::cerr << "Could not load font\n";
        return -1;
    }

    Button rockBtn("Rock", sf::Vector2f(120, 50), 20, sf::Color(65, 105, 225), sf::Color::White);
    rockBtn.setFont(font);
    rockBtn.setPosition({125, 300});

    Button paperBtn("Paper", sf::Vector2f(120, 50), 20, sf::Color(220, 20, 60), sf::Color::White);
    paperBtn.setFont(font);
    paperBtn.setPosition({250, 300});

    Button scissorsBtn("Scissors", sf::Vector2f(120, 50), 20, sf::Color(34, 139, 34), sf::Color::White);
    scissorsBtn.setFont(font);
    scissorsBtn.setPosition({375, 300});

    Network network;
    int playerID = network.getPlayerID();
    
    std::thread msgThread(receiveServerMessages, std::ref(network));
    msgThread.detach();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                std::lock_guard<std::mutex> lock(mtx);
                if (buttonsEnabled) {
                    if (rockBtn.isMouseOver(window)) {
                        gameState = network.sendMove("Rock");
                        buttonsEnabled = false;  // Disable buttons after making a move
                    } else if (paperBtn.isMouseOver(window)) {
                        gameState = network.sendMove("Paper");
                        buttonsEnabled = false;  // Disable buttons after making a move
                    } else if (scissorsBtn.isMouseOver(window)) {
                        gameState = network.sendMove("Scissors");
                        buttonsEnabled = false;  // Disable buttons after making a move
                    }
                }
            }
        }

        window.clear(sf::Color(240, 240, 240));

        rockBtn.setEnabled(buttonsEnabled);
        paperBtn.setEnabled(buttonsEnabled);
        scissorsBtn.setEnabled(buttonsEnabled);

        rockBtn.drawTo(window);
        paperBtn.drawTo(window);
        scissorsBtn.drawTo(window);

        // Draw game state text
        std::stringstream ss(gameState);
        std::string line;
        float yPosition = 100;
        
        while (std::getline(ss, line)) {
            sf::Text gameStateText;
            gameStateText.setFont(font);
            gameStateText.setString(line);
            gameStateText.setCharacterSize(24);
            gameStateText.setFillColor(sf::Color::Black);
            gameStateText.setPosition(
                (window.getSize().x - gameStateText.getGlobalBounds().width) / 2,
                yPosition
            );
            window.draw(gameStateText);
            yPosition += 35;
        }

        // Draw player ID
        sf::Text playerText;
        playerText.setFont(font);
        playerText.setString("Player " + std::to_string(playerID));
        playerText.setCharacterSize(28);
        playerText.setFillColor(sf::Color(50, 50, 50));
        playerText.setStyle(sf::Text::Bold);
        playerText.setPosition(20, 20);
        window.draw(playerText);

        window.display();
    }

    return 0;
}
