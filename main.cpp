#include "game.h"
#include "GraphicalGame.h"
#include <iostream>

int main() {
    std::cout << "=== WARCABY ===\n";
    std::cout << "Wybierz wersje gry:\n";
    std::cout << "1. Wersja konsolowa\n";
    std::cout << "2. Wersja graficzna (SFML)\n";

    int choice;
    std::cin >> choice;

    if (choice == 1) {
        Game game;
        game.play();
    } else if (choice == 2) {

        GraphicalGame graphicalGame;
        graphicalGame.run();
    } else {
        std::cout << "Nieprawidlowy wybor.\n";
    }

    return 0;
}