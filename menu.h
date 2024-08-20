//menu.h
#ifndef MENU_H
#define MENU_H

enum GameSelection {
    NONE,
    TETRIS,
    INVADERS,
};

extern GameSelection currentGame; // Declaración externa de currentGame

void showMenu();
GameSelection handleMenuInput();

#endif
