// game.cpp
#include "gameLogic.h"
#include <cctype>   // For std::toupper

Game::Game(int id) : id(id), p1Gone(false), p2Gone(false), ready(false) {
    moves[1] = "";
    moves[2] = "";
}

bool Game::isConnected() const {
    return ready;
}

bool Game::bothGone() const {
    return p1Gone && p2Gone;
}

std::string Game::getPlayerMove(int player) const {
    return moves[player];
}

void Game::play(int player, const std::string& move) {
    moves[player] = move;
    if (player == 1) {
        p1Gone = true;
    } else {
        p2Gone = true;
    }
}

int Game::findWinner() {
    if (moves[1].empty() || moves[2].empty()) {
        return -1;
    }

    char p1 = std::toupper(moves[1][0]);
    char p2 = std::toupper(moves[2][0]);

    if (p1 == 'R' && p2 == 'P') {
        return 1;
    } else if (p1 == 'R' && p2 == 'S') {
        return 0;
    } else if (p1 == 'P' && p2 == 'R') {
        return 0;
    } else if (p1 == 'P' && p2 == 'S') {
        return 1;
    } else if (p1 == 'S' && p2 == 'R') {
        return 1;
    } else if (p1 == 'S' && p2 == 'P') {
        return 0;
    }

    return -1;
}

void Game::resetGame() {
    p1Gone = false;
    p2Gone = false;
    moves[1] = "";
    moves[2] = "";
}

void Game::setReady() {
    ready = true;
}
