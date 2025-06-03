#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>

enum class PieceType {
    EMPTY = 0,
    WHITE_PAWN = 1,
    BLACK_PAWN = 2,
    WHITE_KING = 3,
    BLACK_KING = 4
};

struct Position {
    int row, col;
    explicit Position(int r = 0, int c = 0) : row(r), col(c) {}
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

struct Move {
    Position from, to;
    std::vector<Position> captured;
    Move(Position f, Position t) : from(f), to(t) {}
};

class Board {
private:
    std::vector<std::vector<PieceType>> board;
    static const int SIZE = 8;

public:
    Board();
    void initializeBoard();
    void displayBoard() const;
    [[nodiscard]] PieceType getPiece(int row, int col) const;
    void setPiece(int row, int col, PieceType piece);
    [[nodiscard]] bool isValidPosition(int row, int col) const;
    [[nodiscard]] bool isDarkSquare(int row, int col) const;
    [[nodiscard]] std::vector<Move> getAllMoves(bool isWhite) const;
    [[nodiscard]] std::vector<Move> getCaptureMoves(bool isWhite) const;
    [[nodiscard]] std::vector<Move> getRegularMoves(bool isWhite) const;
    bool makeMove(const Move& move);
    bool isGameOver(bool& whiteWins) const;
    [[nodiscard]] int countPieces(bool isWhite) const;

private:
    [[nodiscard]] std::vector<Move> getPawnMoves(int row, int col) const;
    [[nodiscard]] std::vector<Move> getKingMoves(int row, int col) const;
    [[nodiscard]] std::vector<Move> getPawnCaptures(int row, int col) const;
    [[nodiscard]] std::vector<Move> getKingCaptures(int row, int col) const;
    [[nodiscard]] bool isWhitePiece(PieceType piece) const;
    [[nodiscard]] bool isBlackPiece(PieceType piece) const;
    [[nodiscard]] bool isKing(PieceType piece) const;
    void promoteToKing(int row, int col);
};

#endif