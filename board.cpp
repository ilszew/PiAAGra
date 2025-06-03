#include "Board.h"
#include <iomanip>

Board::Board() : board(SIZE, std::vector<PieceType>(SIZE, PieceType::EMPTY)) {
    initializeBoard();
}

void Board::initializeBoard() {

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = PieceType::EMPTY;
        }
    }
    

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (isDarkSquare(row, col)) {
                board[row][col] = PieceType::BLACK_PAWN;
            }
        }
    }

    for (int row = 5; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (isDarkSquare(row, col)) {
                board[row][col] = PieceType::WHITE_PAWN;
            }
        }
    }
}

void Board::displayBoard() const {
    std::cout << "\n  ";
    for (int col = 0; col < SIZE; col++) {
        std::cout << "  " << col << " ";
    }
    std::cout << "\n";
    
    for (int row = 0; row < SIZE; row++) {
        std::cout << row << " ";
        for (int col = 0; col < SIZE; col++) {
            if (isDarkSquare(row, col)) {
                char symbol = ' ';
                switch (board[row][col]) {
                    case PieceType::WHITE_PAWN: symbol = 'o'; break;
                    case PieceType::BLACK_PAWN: symbol = 'x'; break;
                    case PieceType::WHITE_KING: symbol = 'O'; break;
                    case PieceType::BLACK_KING: symbol = 'X'; break;
                    default: symbol = '.'; break;
                }
                std::cout << "[" << symbol << "]";
            } else {
                std::cout << "   ";
            }
            std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

PieceType Board::getPiece(int row, int col) const {
    if (isValidPosition(row, col)) {
        return board[row][col];
    }
    return PieceType::EMPTY;
}

void Board::setPiece(int row, int col, PieceType piece) {
    if (isValidPosition(row, col)) {
        board[row][col] = piece;
    }
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

bool Board::isDarkSquare(int row, int col) const {
    return (row + col) % 2 == 1;
}

std::vector<Move> Board::getAllMoves(bool isWhite) const {
    std::vector<Move> captures = getCaptureMoves(isWhite);
    if (!captures.empty()) {
        return captures;
    }
    return getRegularMoves(isWhite);
}

std::vector<Move> Board::getCaptureMoves(bool isWhite) const {
    std::vector<Move> moves;
    
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            PieceType piece = board[row][col];
            if ((isWhite && isWhitePiece(piece)) || (!isWhite && isBlackPiece(piece))) {
                std::vector<Move> pieceMoves;
                if (isKing(piece)) {
                    pieceMoves = getKingCaptures(row, col);
                } else {
                    pieceMoves = getPawnCaptures(row, col);
                }
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    
    return moves;
}

std::vector<Move> Board::getRegularMoves(bool isWhite) const {
    std::vector<Move> moves;
    
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            PieceType piece = board[row][col];
            if ((isWhite && isWhitePiece(piece)) || (!isWhite && isBlackPiece(piece))) {
                std::vector<Move> pieceMoves;
                if (isKing(piece)) {
                    pieceMoves = getKingMoves(row, col);
                } else {
                    pieceMoves = getPawnMoves(row, col);
                }
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    
    return moves;
}

std::vector<Move> Board::getPawnMoves(int row, int col) const {
    std::vector<Move> moves;
    PieceType piece = board[row][col];
    
    int direction = isWhitePiece(piece) ? -1 : 1;

    for (int dcol = -1; dcol <= 1; dcol += 2) {
        int newRow = row + direction;
        int newCol = col + dcol;
        
        if (isValidPosition(newRow, newCol) && isDarkSquare(newRow, newCol) && 
            board[newRow][newCol] == PieceType::EMPTY) {
            moves.push_back(Move(Position(row, col), Position(newRow, newCol)));
        }
    }
    
    return moves;
}

std::vector<Move> Board::getKingMoves(int row, int col) const {
    std::vector<Move> moves;

    int directions[][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    
    for (auto& dir : directions) {
        for (int dist = 1; dist < SIZE; dist++) {
            int newRow = row + dir[0] * dist;
            int newCol = col + dir[1] * dist;
            
            if (!isValidPosition(newRow, newCol) || !isDarkSquare(newRow, newCol)) {
                break;
            }
            
            if (board[newRow][newCol] != PieceType::EMPTY) {
                break;
            }
            
            moves.push_back(Move(Position(row, col), Position(newRow, newCol)));
        }
    }
    
    return moves;
}

std::vector<Move> Board::getPawnCaptures(int row, int col) const {
    std::vector<Move> moves;
    PieceType piece = board[row][col];

    int directions[][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    
    for (auto& dir : directions) {
        int enemyRow = row + dir[0];
        int enemyCol = col + dir[1];
        int landRow = row + 2 * dir[0];
        int landCol = col + 2 * dir[1];
        
        if (isValidPosition(enemyRow, enemyCol) && isValidPosition(landRow, landCol) &&
            isDarkSquare(landRow, landCol) && board[landRow][landCol] == PieceType::EMPTY) {
            
            PieceType enemy = board[enemyRow][enemyCol];
            if ((isWhitePiece(piece) && isBlackPiece(enemy)) ||
                (isBlackPiece(piece) && isWhitePiece(enemy))) {
                
                Move move(Position(row, col), Position(landRow, landCol));
                move.captured.push_back(Position(enemyRow, enemyCol));
                moves.push_back(move);
            }
        }
    }
    
    return moves;
}

std::vector<Move> Board::getKingCaptures(int row, int col) const {
    std::vector<Move> moves;
    PieceType piece = board[row][col];
    
    int directions[][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    
    for (auto& dir : directions) {
        Position enemy(-1, -1);
        bool foundEnemy = false;

        for (int dist = 1; dist < SIZE; dist++) {
            int checkRow = row + dir[0] * dist;
            int checkCol = col + dir[1] * dist;
            
            if (!isValidPosition(checkRow, checkCol) || !isDarkSquare(checkRow, checkCol)) {
                break;
            }
            
            PieceType checkPiece = board[checkRow][checkCol];
            if (checkPiece != PieceType::EMPTY) {
                if (!foundEnemy && ((isWhitePiece(piece) && isBlackPiece(checkPiece)) ||
                                   (isBlackPiece(piece) && isWhitePiece(checkPiece)))) {
                    enemy = Position(checkRow, checkCol);
                    foundEnemy = true;
                } else {
                    break;
                }
            } else if (foundEnemy) {
                Move move(Position(row, col), Position(checkRow, checkCol));
                move.captured.push_back(enemy);
                moves.push_back(move);
            }
        }
    }
    
    return moves;
}

bool Board::makeMove(const Move& move) {
    PieceType piece = board[move.from.row][move.from.col];
    if (piece == PieceType::EMPTY) return false;

    board[move.to.row][move.to.col] = piece;
    board[move.from.row][move.from.col] = PieceType::EMPTY;

    for (const Position& cap : move.captured) {
        board[cap.row][cap.col] = PieceType::EMPTY;
    }

    promoteToKing(move.to.row, move.to.col);
    
    return true;
}

void Board::promoteToKing(int row, int col) {
    PieceType piece = board[row][col];
    if (piece == PieceType::WHITE_PAWN && row == 0) {
        board[row][col] = PieceType::WHITE_KING;
    } else if (piece == PieceType::BLACK_PAWN && row == SIZE - 1) {
        board[row][col] = PieceType::BLACK_KING;
    }
}

bool Board::isGameOver(bool& whiteWins) const {
    bool hasWhite = countPieces(true) > 0;
    bool hasBlack = countPieces(false) > 0;
    
    if (!hasWhite) {
        whiteWins = false;
        return true;
    }
    if (!hasBlack) {
        whiteWins = true;
        return true;
    }

    bool whiteMoves = !getAllMoves(true).empty();
    bool blackMoves = !getAllMoves(false).empty();
    
    if (!whiteMoves) {
        whiteWins = false;
        return true;
    }
    if (!blackMoves) {
        whiteWins = true;
        return true;
    }
    
    return false;
}

int Board::countPieces(bool isWhite) const {
    int count = 0;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            PieceType piece = board[row][col];
            if ((isWhite && isWhitePiece(piece)) || (!isWhite && isBlackPiece(piece))) {
                count++;
            }
        }
    }
    return count;
}

bool Board::isWhitePiece(PieceType piece) const {
    return piece == PieceType::WHITE_PAWN || piece == PieceType::WHITE_KING;
}

bool Board::isBlackPiece(PieceType piece) const {
    return piece == PieceType::BLACK_PAWN || piece == PieceType::BLACK_KING;
}

bool Board::isKing(PieceType piece) const {
    return piece == PieceType::WHITE_KING || piece == PieceType::BLACK_KING;
}