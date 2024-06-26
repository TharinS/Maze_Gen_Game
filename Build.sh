#!/bin/bash

# Compile the source code into an object file
g++ -c Projects/MazeGame.cpp -o Main.o

# Link the object file with the SFML libraries to create the executable
g++ Main.o -o MazeGame -lsfml-graphics -lsfml-window -lsfml-system

# Run the executable
./MazeGame
