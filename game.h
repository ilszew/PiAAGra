#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include <random>

class Game {
private:
    Board board;
    bool playerTurn; // true = gracz (białe), false = komputer (czarne)
    std::mt19937 rng;

public:
    Game();
    void play();
    void playerMove();
    void computerMove();
    bool getPlayerInput(Position& from, Position& to);
    void displayGameState();
    bool isValidPlayerMove(const Position& from, const Position& to, Move& validMove);
    Move getBestComputerMove();
    [[nodiscard]] int evaluateBoard() const;
    [[nodiscard]] int minimax(Board tempBoard, int depth, bool maximizing) const;
    void displayInstructions() const;
};

#endif