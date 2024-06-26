#pragma once
#include <stdlib.h>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

using namespace sf;
using namespace std;

#define CELLCOUNT 16
#define WINDOWDIMENSIONS 800
#define CELLSIZE WINDOWDIMENSIONS / CELLCOUNT

typedef struct int4values {
    bool Top_present = false, Right_present = false, Left_present = false, Bottom_present = false;
    int Top = 0, Right = 0, Left = 0, Bottom = 0;  // T,R,B,L
} Int4;

typedef struct bool4values {
    bool x = false, y = false, z = false, w = false;
} Bool4;

bool HasPlayerLost(int playerIndex, int enemyIndex, bool enemyState) { return ((playerIndex == enemyIndex) && !enemyState) ? true : false; }

bool HasPlayerWon(int playerIndexState) { return (playerIndexState == 2) ? true : false; }

int returnCELLID(int x, int y) { return (x) + (CELLCOUNT * (y)); }

Vector2i returnCoordinates(int CellID) { return Vector2i(CellID % CELLCOUNT, CellID / CELLCOUNT); }