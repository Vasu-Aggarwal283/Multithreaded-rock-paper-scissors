// game.h
#ifndef GAME_H
#define GAME_H

#include <string>

class Game {
public:
    Game(int id);
    bool isConnected() const;
    bool bothGone() const;
    std::string getPlayerMove(int player) const;
    void play(int player, const std::string& move);
    int findWinner();
    void resetGame();
    void setReady();

private:
    int id;
    bool p1Gone;
    bool p2Gone;
    bool ready;
    std::string moves[3];
};

#endif // GAME_H
