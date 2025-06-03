#include "Game.h"
#include <iostream>
#include <algorithm>
#include <climits>

Game::Game() : playerTurn(true), rng(std::random_device{}()) {}

void Game::play() {
    displayInstructions();
    
    while (true) {
        displayGameState();
        
        bool whiteWins;
        if (board.isGameOver(whiteWins)) {
            std::cout << "\n=== KONIEC GRY ===\n";
            if (whiteWins) {
                std::cout << "Wygrales!\n";
            } else {
                std::cout << "Komputer wygral\n";
            }
            break;
        }
        
        if (playerTurn) {
            std::cout << "\nTwoj ruch (biale pionki 'o'):\n";
            playerMove();
        } else {
            std::cout << "\nRuch komputera (czarne pionki 'x')...\n";
            computerMove();
        }
        
        playerTurn = !playerTurn;
    }
}

void Game::displayInstructions() const {
    std::cout << "=== WARCABY ===\n";
    std::cout << "Zasady:\n";
    std::cout << "- Twoje pionki: 'o' (biale), damki: 'O'\n";
    std::cout << "- Pionki komputera: 'x' (czarne), damki: 'X'\n";
    std::cout << "- Bicie jest obowiazkowe\n";
    std::cout << "- Pionek staje sie damka na przeciwnej stronie\n";
    std::cout << "- Podaj wspolrzedne: wiersz kolumna (np. 5 2)\n";
    std::cout << "- Najpierw skad, potem dokad\n\n";
}

void Game::displayGameState() {
    board.displayBoard();
    std::cout << "Biale pionki 'o': " << board.countPieces(true)
              << " | Czarne pionki 'x': " << board.countPieces(false) << "\n";
}

void Game::playerMove() {
    Position from, to;
    
    while (true) {
        if (!getPlayerInput(from, to)) {
            std::cout << "Nieprawidlowe dane. Sprobuj ponownie.\n";
            continue;
        }
        
        Move validMove(from, to);
        if (isValidPlayerMove(from, to, validMove)) {
            board.makeMove(validMove);
            std::cout << "Ruch wykonany: (" << from.row << "," << from.col 
                      << ") -> (" << to.row << "," << to.col << ")\n";
            break;
        } else {
            std::cout << "Nieprawidlowy ruch. Sprobuj ponownie.\n";
        }
    }
}

bool Game::getPlayerInput(Position& from, Position& to) {
    std::cout << "Skad (wiersz kolumna): ";
    if (!(std::cin >> from.row >> from.col)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return false;
    }
    
    std::cout << "Dokad (wiersz kolumna): ";
    if (!(std::cin >> to.row >> to.col)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return false;
    }
    
    return true;
}

bool Game::isValidPlayerMove(const Position& from, const Position& to, Move& validMove) {
    std::vector<Move> possibleMoves = board.getAllMoves(true);
    
    for (const Move& move : possibleMoves) {
        if (move.from == from && move.to == to) {
            validMove = move;
            return true;
        }
    }
    
    return false;
}

void Game::computerMove() {
    Move bestMove = getBestComputerMove();
    board.makeMove(bestMove);
    std::cout << "Komputer: (" << bestMove.from.row << "," << bestMove.from.col 
              << ") -> (" << bestMove.to.row << "," << bestMove.to.col << ")\n";
}

Move Game::getBestComputerMove() {
    std::vector<Move> possibleMoves = board.getAllMoves(false);
    
    if (possibleMoves.empty()) {
        return Move(Position(), Position());
    }
    
    Move bestMove = possibleMoves[0];
    int bestScore = INT_MIN;
    
    for (const Move& move : possibleMoves) {
        Board tempBoard = board;
        tempBoard.makeMove(move);
        
        int score = minimax(tempBoard, 3, false);
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    
    return bestMove;
}

int Game::minimax(Board tempBoard, int depth, bool maximizing) const {
    bool whiteWins;
    if (depth == 0 || tempBoard.isGameOver(whiteWins)) {
        if (tempBoard.isGameOver(whiteWins)) {
            return whiteWins ? -1000 : 1000;
        }
        return evaluateBoard();
    }
    
    if (maximizing) {
        int maxEval = INT_MIN;
        std::vector<Move> moves = tempBoard.getAllMoves(false);
        
        for (const Move& move : moves) {
            Board newBoard = tempBoard;
            newBoard.makeMove(move);
            int eval = minimax(newBoard, depth - 1, false);
            maxEval = std::max(maxEval, eval);
        }
        
        return maxEval;
    } else {
        int minEval = INT_MAX;
        std::vector<Move> moves = tempBoard.getAllMoves(true);
        
        for (const Move& move : moves) {
            Board newBoard = tempBoard;
            newBoard.makeMove(move);
            int eval = minimax(newBoard, depth - 1, true);
            minEval = std::min(minEval, eval);
        }
        
        return minEval;
    }
}

int Game::evaluateBoard() const {
    int score = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            PieceType piece = board.getPiece(row, col);
            
            switch (piece) {
                case PieceType::BLACK_PAWN:
                    score += 10;
                    break;
                case PieceType::BLACK_KING:
                    score += 30;
                    break;
                case PieceType::WHITE_PAWN:
                    score -= 10;
                    break;
                case PieceType::WHITE_KING:
                    score -= 30;
                    break;
                default:
                    break;
            }
        }
    }

    for (int row = 2; row < 6; row++) {
        for (int col = 2; col < 6; col++) {
            PieceType piece = board.getPiece(row, col);
            if (piece == PieceType::BLACK_PAWN || piece == PieceType::BLACK_KING) {
                score += 2;
            } else if (piece == PieceType::WHITE_PAWN || piece == PieceType::WHITE_KING) {
                score -= 2;
            }
        }
    }
    
    return score;
}