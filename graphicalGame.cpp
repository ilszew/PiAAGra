#include "GraphicalGame.h"
#include <iostream>
#include <algorithm>
#include <climits>
#include <sstream>

GraphicalGame::GraphicalGame() 
    : playerTurn(true), rng(std::random_device{}()), 
      selectedPiece(-1, -1), pieceSelected(false), gameRunning(true), showInstructions(true),
      window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Warcaby - Graficzna wersja") {
    

    LIGHT_SQUARE_COLOR = sf::Color(240, 217, 181);
    DARK_SQUARE_COLOR = sf::Color(181, 136, 99);
    WHITE_PIECE_COLOR = sf::Color(255, 255, 255);
    BLACK_PIECE_COLOR = sf::Color(50, 50, 50);
    HIGHLIGHT_COLOR = sf::Color(255, 255, 0, 150);
    SELECTED_COLOR = sf::Color(0, 255, 0, 150);
    POSSIBLE_MOVE_COLOR = sf::Color(0, 0, 255, 100);
}

void GraphicalGame::run() {
    if (!initializeGraphics()) {
        std::cerr << "blad inicjalizacji grafiki" << std::endl;
        return;
    }

    window.setFramerateLimit(60);
    
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

bool GraphicalGame::initializeGraphics() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Nie mozna zaladować orginalnej czcionki, uzywam domyslnej " << std::endl;
    }
    
    return true;
}

void GraphicalGame::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
                
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (showInstructions) {
                        showInstructions = false;
                    } else if (gameRunning && playerTurn) {
                        handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                    }
                }
                break;
                
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::R) {
                    // Restart gry
                    board = Board();
                    playerTurn = true;
                    gameRunning = true;
                    resetSelection();
                }
                if (event.key.code == sf::Keyboard::H) {
                    showInstructions = !showInstructions;
                }
                break;
                
            default:
                break;
        }
    }
}

void GraphicalGame::handleMouseClick(int mouseX, int mouseY) {
    Position clickedSquare = getSquareFromPixel(mouseX, mouseY);

    if (clickedSquare.row < 0 || clickedSquare.row >= 8 || 
        clickedSquare.col < 0 || clickedSquare.col >= 8) {
        return;
    }

    if (!board.isDarkSquare(clickedSquare.row, clickedSquare.col)) {
        return;
    }
    
    if (!pieceSelected) {
        PieceType piece = board.getPiece(clickedSquare.row, clickedSquare.col);
        if ((piece == PieceType::WHITE_PAWN || piece == PieceType::WHITE_KING)) {
            selectPiece(clickedSquare);
        }
    } else {
        if (clickedSquare == selectedPiece) {
            resetSelection();
        } else {
            PieceType piece = board.getPiece(clickedSquare.row, clickedSquare.col);
            if (piece == PieceType::WHITE_PAWN || piece == PieceType::WHITE_KING) {
                selectPiece(clickedSquare);
            } else {
                makePlayerMove(clickedSquare);
            }
        }
    }
}

Position GraphicalGame::getSquareFromPixel(int pixelX, int pixelY) {
    int col = (pixelX - BOARD_OFFSET_X) / SQUARE_SIZE;
    int row = (pixelY - BOARD_OFFSET_Y) / SQUARE_SIZE;
    return Position(row, col);
}

sf::Vector2f GraphicalGame::getPixelFromSquare(const Position& pos) {
    float x = BOARD_OFFSET_X + pos.col * SQUARE_SIZE + SQUARE_SIZE / 2.0f;
    float y = BOARD_OFFSET_Y + pos.row * SQUARE_SIZE + SQUARE_SIZE / 2.0f;
    return sf::Vector2f(x, y);
}

void GraphicalGame::selectPiece(const Position& pos) {
    selectedPiece = pos;
    pieceSelected = true;
    updatePossibleMoves();
}

void GraphicalGame::makePlayerMove(const Position& to) {
    Move validMove(selectedPiece, to);
    if (isValidPlayerMove(selectedPiece, to, validMove)) {
        board.makeMove(validMove);
        playerTurn = false;
        resetSelection();
    }
}

void GraphicalGame::resetSelection() {
    pieceSelected = false;
    selectedPiece = Position(-1, -1);
    possibleMoves.clear();
}

void GraphicalGame::updatePossibleMoves() {
    possibleMoves.clear();
    if (pieceSelected) {
        std::vector<Move> allMoves = board.getAllMoves(true);
        for (const Move& move : allMoves) {
            if (move.from == selectedPiece) {
                possibleMoves.push_back(move);
            }
        }
    }
}

bool GraphicalGame::isPossibleMove(const Position& pos) const {
    for (const Move& move : possibleMoves) {
        if (move.to == pos) {
            return true;
        }
    }
    return false;
}

bool GraphicalGame::isValidPlayerMove(const Position& from, const Position& to, Move& validMove) {
    std::vector<Move> allMoves = board.getAllMoves(true);
    
    for (const Move& move : allMoves) {
        if (move.from == from && move.to == to) {
            validMove = move;
            return true;
        }
    }
    
    return false;
}

void GraphicalGame::update() {
    if (!gameRunning) return;

    bool whiteWins;
    if (board.isGameOver(whiteWins)) {
        gameRunning = false;
        return;
    }

    if (!playerTurn && gameRunning) {
        computerMove();
        playerTurn = true;
    }
}

void GraphicalGame::computerMove() {
    Move bestMove = getBestComputerMove();
    if (bestMove.from.row != -1) {
        board.makeMove(bestMove);
    }
}

Move GraphicalGame::getBestComputerMove() {
    std::vector<Move> possibleMoves = board.getAllMoves(false);
    
    if (possibleMoves.empty()) {
        return Move(Position(-1, -1), Position(-1, -1));
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

int GraphicalGame::minimax(Board tempBoard, int depth, bool maximizing) const {
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

int GraphicalGame::evaluateBoard() const {
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

void GraphicalGame::render() {
    window.clear(sf::Color(50, 50, 50));
    
    if (showInstructions) {
        drawInstructions();
    } else {
        drawBoard();
        drawHighlights();
        drawPieces();
        drawUI();
        
        if (!gameRunning) {
            bool whiteWins;
            board.isGameOver(whiteWins);
            drawGameOverScreen(whiteWins);
        }
    }
    
    window.display();
}

void GraphicalGame::drawBoard() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            square.setPosition(BOARD_OFFSET_X + col * SQUARE_SIZE, 
                             BOARD_OFFSET_Y + row * SQUARE_SIZE);
            
            if (board.isDarkSquare(row, col)) {
                square.setFillColor(DARK_SQUARE_COLOR);
            } else {
                square.setFillColor(LIGHT_SQUARE_COLOR);
            }
            
            window.draw(square);
        }
    }
}

void GraphicalGame::drawHighlights() {
    if (pieceSelected) {
        sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        highlight.setPosition(BOARD_OFFSET_X + selectedPiece.col * SQUARE_SIZE,
                            BOARD_OFFSET_Y + selectedPiece.row * SQUARE_SIZE);
        highlight.setFillColor(SELECTED_COLOR);
        window.draw(highlight);
    }

    for (const Move& move : possibleMoves) {
        sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        highlight.setPosition(BOARD_OFFSET_X + move.to.col * SQUARE_SIZE,
                            BOARD_OFFSET_Y + move.to.row * SQUARE_SIZE);
        highlight.setFillColor(POSSIBLE_MOVE_COLOR);
        window.draw(highlight);
    }
}

void GraphicalGame::drawPieces() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            PieceType piece = board.getPiece(row, col);
            if (piece != PieceType::EMPTY) {
                sf::Vector2f center = getPixelFromSquare(Position(row, col));

                sf::CircleShape pieceShape(PIECE_RADIUS);
                pieceShape.setOrigin(PIECE_RADIUS, PIECE_RADIUS);
                pieceShape.setPosition(center);

                if (piece == PieceType::WHITE_PAWN || piece == PieceType::WHITE_KING) {
                    pieceShape.setFillColor(WHITE_PIECE_COLOR);
                    pieceShape.setOutlineColor(sf::Color::Black);
                } else {
                    pieceShape.setFillColor(BLACK_PIECE_COLOR);
                    pieceShape.setOutlineColor(sf::Color::White);
                }
                pieceShape.setOutlineThickness(2);
                
                window.draw(pieceShape);

                if (piece == PieceType::WHITE_KING || piece == PieceType::BLACK_KING) {
                    sf::CircleShape crown(PIECE_RADIUS - 10);
                    crown.setOrigin(PIECE_RADIUS - 10, PIECE_RADIUS - 10);
                    crown.setPosition(center);
                    crown.setFillColor(sf::Color(255, 215, 0));
                    crown.setOutlineColor(sf::Color::Black);
                    crown.setOutlineThickness(1);
                    window.draw(crown);
                }
            }
        }
    }
}

void GraphicalGame::drawUI() {
    sf::RectangleShape statusBar(sf::Vector2f(WINDOW_WIDTH, 50));
    statusBar.setPosition(0, 0);
    statusBar.setFillColor(sf::Color(30, 30, 30));
    window.draw(statusBar);

    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(20);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(10, 15);
    
    std::string status = "Biale: " + std::to_string(board.countPieces(true)) + 
                        " | Czarne: " + std::to_string(board.countPieces(false));
    
    if (gameRunning) {
        status += " | Tura: " + std::string(playerTurn ? "Gracza" : "Komputera");
    }
    
    statusText.setString(status);
    window.draw(statusText);

    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(16);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(10, WINDOW_HEIGHT - 40);
    helpText.setString("R - Reset | H - Pomoc | Kliknij pionek, potem cel");
    window.draw(helpText);
}

void GraphicalGame::drawInstructions() {
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(background);

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::White);
    title.setString("WARCABY");
    title.setPosition(WINDOW_WIDTH/2 - 100, 50);
    window.draw(title);

    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(18);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(50, 150);
    
    std::string instructionText = 
        "ZASADY GRY:\n\n"
        "- Twoje pionki: biale (o), damki ze zlotym kolkiem\n"
        "- Pionki komputera: czarne (x), damki ze zlotym kolkiem\n"
        "- Poruszasz sie tylko po ciemnych polach\n"
        "- Bicie jest obowiazkowe gdy jest mozliwe\n"
        "- Pionek staje sie damka na przeciwnej stronie planszy\n"
        "- Damki moga poruszac sie we wszystkich kierunkach\n\n"
        "STEROWANIE:\n\n"
        "- Kliknij na swoj pionek aby go wybrac\n"
        "- Kliknij na cel aby wykonac ruch\n"
        "- Mozliwe ruchy sa podswietlone na niebiesko\n"
        "- Wybrany pionek jest podswietlony na zielono\n\n"
        "KLAWISZE:\n\n"
        "- R - Reset gry\n"
        "- H - Pokaz/ukryj pomoc\n\n"
        "Kliknij gdziekolwiek aby zaczac gre";
    
    instructions.setString(instructionText);
    window.draw(instructions);
}

void GraphicalGame::drawGameOverScreen(bool whiteWins) {
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    sf::RectangleShape resultBox(sf::Vector2f(400, 200));
    resultBox.setPosition(WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 - 100);
    resultBox.setFillColor(sf::Color(50, 50, 50));
    resultBox.setOutlineColor(sf::Color::White);
    resultBox.setOutlineThickness(3);
    window.draw(resultBox);

    sf::Text resultText;
    resultText.setFont(font);
    resultText.setCharacterSize(30);
    resultText.setFillColor(sf::Color::White);
    
    std::string result = whiteWins ? "WYGRALEs!" : "KOMPUTER WYGRAL";
    resultText.setString(result);

    sf::FloatRect textBounds = resultText.getLocalBounds();
    resultText.setPosition(WINDOW_WIDTH/2 - textBounds.width/2, WINDOW_HEIGHT/2 - 50);
    window.draw(resultText);

    sf::Text restartText;
    restartText.setFont(font);
    restartText.setCharacterSize(18);
    restartText.setFillColor(sf::Color::White);
    restartText.setString("Nacisnij R aby zagrac ponownie");
    
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setPosition(WINDOW_WIDTH/2 - restartBounds.width/2, WINDOW_HEIGHT/2 + 20);
    window.draw(restartText);
}