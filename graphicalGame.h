#ifndef GRAPHICALGAME_H
#define GRAPHICALGAME_H

#include "Board.h"
#include <SFML/Graphics.hpp>
#include <random>

class GraphicalGame {
private:
    Board board;
    bool playerTurn;
    std::mt19937 rng;
    
    sf::RenderWindow window;
    sf::Font font;

    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 900;
    static const int BOARD_SIZE = 800;
    static const int SQUARE_SIZE = BOARD_SIZE / 8;
    static const int BOARD_OFFSET_X = 0;
    static const int BOARD_OFFSET_Y = 50;
    static const int PIECE_RADIUS = 35;

    sf::Color LIGHT_SQUARE_COLOR;
    sf::Color DARK_SQUARE_COLOR;
    sf::Color WHITE_PIECE_COLOR;
    sf::Color BLACK_PIECE_COLOR;
    sf::Color HIGHLIGHT_COLOR;
    sf::Color SELECTED_COLOR;
    sf::Color POSSIBLE_MOVE_COLOR;

    Position selectedPiece;
    bool pieceSelected;
    std::vector<Move> possibleMoves;
    bool gameRunning;
    bool showInstructions;
    
public:
    GraphicalGame();
    void run();
    
private:
    bool initializeGraphics();
    void handleEvents();
    void update();
    void render();

    void handleMouseClick(int mouseX, int mouseY);
    Position getSquareFromPixel(int pixelX, int pixelY);
    sf::Vector2f getPixelFromSquare(const Position& pos);

    void drawBoard();
    void drawPieces();
    void drawHighlights();
    void drawUI();
    void drawInstructions();
    void drawGameOverScreen(bool whiteWins);

    void selectPiece(const Position& pos);
    void makePlayerMove(const Position& to);
    void computerMove();
    Move getBestComputerMove();
    int evaluateBoard() const;
    int minimax(Board tempBoard, int depth, bool maximizing) const;

    bool isValidPlayerMove(const Position& from, const Position& to, Move& validMove);
    void updatePossibleMoves();

    void resetSelection();
    bool isPossibleMove(const Position& pos) const;
    std::string formatText(const std::string& text, int maxWidth) const;
};

#endif