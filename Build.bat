g++ -I src/include -c Projects/MazeGame.cpp -o Main.o
g++ Main.o -o PacMan -L src/lib -lsfml-graphics -lsfml-window -lsfml-system 
PacMan.exe