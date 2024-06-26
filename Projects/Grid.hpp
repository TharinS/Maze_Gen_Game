#include "General.hpp"

using namespace sf;
using namespace std;

/**
 * Stack class maintains the stack used in the backtracking algorithm for generating the maze
 *
 * Methods -
 * push allocates space in memory for a new node and then pushes it into the stack
 * pop take the first node in the stack and returns the cellID or 0 if there is no node(NULL) and updates the stack
 */
class Stack {
   private:
    typedef struct _stackStruct {
        int value;
        struct _stackStruct *Next;
    } stack;

    typedef stack *StackPtr;

    StackPtr _stack;

    void push(StackPtr *Stack, int value) {
        StackPtr NewNode = (stack *)malloc(sizeof(stack));
        NewNode->value = value;
        NewNode->Next = *Stack;
        *Stack = NewNode;
    }

    int pop(StackPtr *Stack) {
        if (!(*Stack == NULL)) {
            int value = (*Stack)->value;
            StackPtr temp = *Stack;
            *Stack = _stack->Next;
            free(temp);
            return value;
        } else {
            return 0;
        }
    }

   public:
    Stack() {
        _stack = NULL;
    }

    void Push(int value) {
        push(&_stack, value);
    }

    int Pop() {
        return pop(&_stack);
    }
};

class Wall {
   public:
    struct WALLS {
        bool TOP;     // 0
        bool RIGHT;   // 1
        bool BOTTOM;  // 2
        bool LEFT;    // 3
    } wall_configuration;

    RectangleShape Walls[4];
};

class Cell {
   private:
    Wall _cellWalls;
    RectangleShape _cell;

    enum CELLEVENTS {
        PATH,
        START,
        END
    } _state;

    void GenerateWalls(int x, int y, int width, int height) {
        _cellWalls.Walls[0].setPosition(Vector2f(x, y - 2));
        _cellWalls.Walls[0].setSize(Vector2f(width, 2));
        _cellWalls.Walls[0].setFillColor(Color::Blue);
        _cellWalls.wall_configuration.TOP = true;

        _cellWalls.Walls[1].setPosition(Vector2f(x + width + 2, y));
        _cellWalls.Walls[1].setSize(Vector2f(width, 2));
        _cellWalls.Walls[1].rotate(90);
        _cellWalls.Walls[1].setFillColor(Color::Blue);
        _cellWalls.wall_configuration.RIGHT = true;

        _cellWalls.Walls[2].setPosition(Vector2f(x, y + height + 2));
        _cellWalls.Walls[2].setSize(Vector2f(width, 2));
        _cellWalls.Walls[2].setFillColor(Color::Blue);
        _cellWalls.wall_configuration.BOTTOM = true;

        _cellWalls.Walls[3].setPosition(Vector2f(x - 2, y));
        _cellWalls.Walls[3].setSize(Vector2f(width, 2));
        _cellWalls.Walls[3].rotate(90);
        _cellWalls.Walls[3].setFillColor(Color::Blue);
        _cellWalls.wall_configuration.LEFT = true;
    }

    void UpdateWalls() {
        if (_cellWalls.wall_configuration.TOP) {
            _cellWalls.Walls[0].setFillColor(Color::Blue);
        } else {
            _cellWalls.Walls[0].setFillColor(Color::Black);
        }
        if (_cellWalls.wall_configuration.RIGHT) {
            _cellWalls.Walls[1].setFillColor(Color::Blue);
        } else {
            _cellWalls.Walls[1].setFillColor(Color::Black);
        }
        if (_cellWalls.wall_configuration.BOTTOM) {
            _cellWalls.Walls[2].setFillColor(Color::Blue);
        } else {
            _cellWalls.Walls[2].setFillColor(Color::Black);
        }
        if (_cellWalls.wall_configuration.LEFT) {
            _cellWalls.Walls[3].setFillColor(Color::Blue);
        } else {
            _cellWalls.Walls[3].setFillColor(Color::Black);
        }
    }

    void UpdateCellState() {
        switch (_state) {
            case 0:
                _cell.setFillColor(Color::Black);
                break;
            case 1:
                _cell.setFillColor(Color::Red);
                break;
            case 2:
                _cell.setFillColor(Color::Green);
                break;
        }
    }

   public:
    bool visited;

    Cell() {
        _state = PATH;
        visited = false;
        _cell.setFillColor(Color::Black);
        _cell.setSize(Vector2f(CELLSIZE, CELLSIZE));
        _cell.setPosition(Vector2f(0, 0));
        GenerateWalls(0, 0, CELLSIZE, CELLSIZE);
    }

    Cell(int x, int y, int height, int width, Color color) {
        _state = PATH;
        visited = false;
        _cell.setFillColor(color);
        _cell.setSize(Vector2f(CELLSIZE - 3, CELLSIZE - 3));
        _cell.setPosition(Vector2f(x, y));
        GenerateWalls(x, y, CELLSIZE - 3, CELLSIZE - 3);
    }

    Bool4 GetWallConfig() {
        Bool4 WallConfig;
        if (_cellWalls.wall_configuration.TOP) {
            WallConfig.x = true;
        }
        if (_cellWalls.wall_configuration.RIGHT) {
            WallConfig.y = true;
        }
        if (_cellWalls.wall_configuration.BOTTOM) {
            WallConfig.z = true;
        }
        if (_cellWalls.wall_configuration.LEFT) {
            WallConfig.w = true;
        }
        return WallConfig;
    }

    int GetCellState() {
        return _state;
    }

    void SetState(int ID) {
        switch (ID) {
            case 0:
                _state = PATH;
                break;
            case 1:
                _state = START;
                break;
            case 2:
                _state = END;
                break;
        }
        UpdateCellState();
    }

    Vector2f ReturnCenter() {
        return Vector2f(_cell.getGlobalBounds().left + _cell.getGlobalBounds().width / 2, _cell.getGlobalBounds().top + _cell.getGlobalBounds().height / 2);
    }

    void RemoveWall(int ID) {
        switch (ID) {
            case 0:
                _cellWalls.wall_configuration.TOP = false;
                break;
            case 1:
                _cellWalls.wall_configuration.RIGHT = false;
                break;
            case 2:
                _cellWalls.wall_configuration.BOTTOM = false;
                break;
            case 3:
                _cellWalls.wall_configuration.LEFT = false;
                break;
        }
        UpdateWalls();
    }

    void ResetCell(int x, int y, int height, int width, Color color) {
        visited = false;
        _cell.setFillColor(color);
        _cell.setSize(Vector2f(CELLSIZE - 3, CELLSIZE - 3));
        _cell.setPosition(Vector2f(x, y));
        GenerateWalls(x, y, CELLSIZE - 3, CELLSIZE - 3);
    }

    RectangleShape CELL() {
        return _cell;
    }

    RectangleShape WALL(int index) {
        return _cellWalls.Walls[index];
    }
};

class Grid {
   private:
    Stack _visitedCells;

    Int4 noOfNeighbour(Cell Array[], int CELL_ID) {
        Int4 neighbours;
        Vector2i Coordinates = returnCoordinates(CELL_ID);
        if (Coordinates.y - 1 >= 0 && !(Array[returnCELLID(Coordinates.x, Coordinates.y - 1)].visited)) {
            neighbours.Top_present = true;
            neighbours.Top = returnCELLID(Coordinates.x, Coordinates.y - 1);
        }
        if (Coordinates.y + 1 < CELLCOUNT && !(Array[returnCELLID(Coordinates.x, Coordinates.y + 1)].visited)) {
            neighbours.Bottom_present = true;
            neighbours.Bottom = returnCELLID(Coordinates.x, Coordinates.y + 1);
        }
        if (Coordinates.x - 1 >= 0 && !(Array[returnCELLID(Coordinates.x - 1, Coordinates.y)].visited)) {
            neighbours.Left_present = true;
            neighbours.Left = returnCELLID(Coordinates.x - 1, Coordinates.y);
        }
        if (Coordinates.x + 1 < CELLCOUNT && !(Array[returnCELLID(Coordinates.x + 1, Coordinates.y)].visited)) {
            neighbours.Right_present = true;
            neighbours.Right = returnCELLID(Coordinates.x + 1, Coordinates.y);
        }

        return neighbours;
    }

    Vector3i randomNeighbour(Int4 NeighboursList) {
        int i = 0;
        Vector3i List[4];
        Vector3i ReturnValue(CELLCOUNT * CELLCOUNT, 0, 0);  // CellID | From | To
        if (NeighboursList.Bottom_present) {
            List[i].x = NeighboursList.Bottom;
            List[i].y = 2;
            List[i].z = 0;
            i++;
        }
        if (NeighboursList.Top_present) {
            List[i].x = NeighboursList.Top;
            List[i].y = 0;
            List[i].z = 2;
            i++;
        }
        if (NeighboursList.Left_present) {
            List[i].x = NeighboursList.Left;
            List[i].y = 3;
            List[i].z = 1;
            i++;
        }
        if (NeighboursList.Right_present) {
            List[i].x = NeighboursList.Right;
            List[i].y = 1;
            List[i].z = 3;
            i++;
        }
        if (i == 0) {
            return ReturnValue;
        }
        random_shuffle(&List[0], &List[i]);
        ReturnValue = List[rand() % i];
        return ReturnValue;
    }

    void GenerateMaze() {
        srand(time(NULL));
        int No_Visited_Cells = CELLCOUNT * CELLCOUNT;
        int Previous_Cell_ID = returnCELLID(rand() % CELLCOUNT, rand() % CELLCOUNT);

        while (No_Visited_Cells != 0) {
            Int4 Neighbours = noOfNeighbour(grid, Previous_Cell_ID);
            Vector3i Cell_Number = randomNeighbour(Neighbours);
            if (Cell_Number.x == CELLCOUNT * CELLCOUNT) {
                grid[Previous_Cell_ID].visited = true;
                Previous_Cell_ID = _visitedCells.Pop();
                --No_Visited_Cells;
            } else {
                grid[Cell_Number.x].visited = true;
                grid[Previous_Cell_ID].RemoveWall(Cell_Number.y);
                grid[Cell_Number.x].RemoveWall(Cell_Number.z);
                _visitedCells.Push(Cell_Number.x);
                Previous_Cell_ID = Cell_Number.x;
            }
        }
    }

   public:
    // Slow but works
    Cell grid[CELLCOUNT * CELLCOUNT];

    Grid() {
        int xPos = 0;
        int yPos = 0;
        for (int x = 0; x < CELLCOUNT; x++) {
            for (int y = 0; y < CELLCOUNT; y++) {
                int cellID = x * CELLCOUNT + y;
                grid[cellID] = Cell(xPos, yPos, CELLCOUNT, CELLCOUNT, Color::Black);
                xPos += CELLSIZE;
            }
            yPos += CELLSIZE;
            xPos = 0;
        }
        GenerateMaze();
        // Setting the first and last cell to be the start and end points
        grid[0].SetState(1);
        grid[(CELLCOUNT * CELLCOUNT) - 1].SetState(2);
    }

    Int4 GetNeighbours(int Index) {
        Int4 neighbours;
        Vector2i Coordinates = returnCoordinates(Index);
        if (Coordinates.y - 1 >= 0 && !(grid[Index].GetWallConfig().x)) {
            neighbours.Top_present = true;
            neighbours.Top = (Coordinates.y - 1) * CELLCOUNT + Coordinates.x;
        }
        if (Coordinates.y + 1 < CELLCOUNT && !(grid[Index].GetWallConfig().z)) {
            neighbours.Bottom_present = true;
            neighbours.Bottom = (Coordinates.y + 1) * CELLCOUNT + Coordinates.x;
        }
        if (Coordinates.x - 1 >= 0 && !(grid[Index].GetWallConfig().w)) {
            neighbours.Left_present = true;
            neighbours.Left = Coordinates.y * CELLCOUNT + (Coordinates.x - 1);
        }
        if (Coordinates.x + 1 < CELLCOUNT && !(grid[Index].GetWallConfig().y)) {
            neighbours.Right_present = true;
            neighbours.Right = Coordinates.y * CELLCOUNT + (Coordinates.x + 1);
        }
        return neighbours;
    }

    Bool4 GetCellStates(int Index) {
        Bool4 neighbours;
        Vector2i Coordinates = returnCoordinates(Index);
        if (Coordinates.y - 1 >= 0) {
            if (grid[Coordinates.y - 1].GetCellState() == 0) {
                neighbours.x = true;
            }
        }
        if (Coordinates.y + 1 < CELLCOUNT) {
            if (grid[Coordinates.y + 1].GetCellState() == 0) {
                neighbours.z = true;
            }
        }
        if (Coordinates.x - 1 >= 0) {
            if (grid[Coordinates.x - 1].GetCellState() == 0) {
                neighbours.w = true;
            }
        }
        if (Coordinates.x + 1 < CELLCOUNT) {
            if (grid[Coordinates.x + 1].GetCellState() == 0) {
                neighbours.y = true;
            }
        }
        return neighbours;
    }

    RectangleShape ReturnCell(int Index) {
        return grid[Index].CELL();
    }

    RectangleShape ReturnWall(int cellIndex, int wallIndex) {
        return grid[cellIndex].WALL(wallIndex);
    }
};