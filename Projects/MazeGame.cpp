#include <stdlib.h>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

////////////////GENERAL SETTINGS AND FUNCTIONS///////////////////////////////////

#define CELLCOUNT 16                           // defines the size of the grid CELLCOUNT by CELLCOUNT
#define WINDOWDIMENSIONS 800                   // defines the dimensions of the window WINDOWDIMENSIONS by WINDOWDIMENSIONS
#define CELLSIZE WINDOWDIMENSIONS / CELLCOUNT  // defines how to calculate the cell size

/**
 * @brief struct containing 4 boolean vairables that specify whether there are neighbours present in a particular direction and 4 integer variables that hold the index of these neigbours.
 */
typedef struct int4values {
    bool Top_present = false, Right_present = false, Left_present = false, Bottom_present = false;
    int Top = 0, Right = 0, Left = 0, Bottom = 0;  // T,R,B,L
} Int4;

/**
 * @brief struct containing 4 boolean variables that specify whether there are neighbours present in a particular direction.
 */
typedef struct bool4values {
    bool x = false, y = false, z = false, w = false;
} Bool4;

/**
 * @brief checks whether the player is going to lose by seeing if the player is in the same cell as an enemy and if so checks if the enemy is in engage or attack mode.
 *
 * @param playerIndex
 * @param enemyIndex
 * @param enemyState
 * @return true
 * @return false
 */
bool HasPlayerLost(int playerIndex, int enemyIndex, bool enemyState) { return ((playerIndex == enemyIndex) && !enemyState) ? true : false; }

/**
 * @brief Checks whether the player is going to win by seeing if the player is in the end cell.
 *
 * @param playerIndexState
 * @return true
 * @return false
 */
bool HasPlayerWon(int playerIndexState) { return (playerIndexState == 2) ? true : false; }

/**
 * @brief Converts coordinates into a cell index and returns it.
 *
 * @param x
 * @param y
 * @return int
 */
int returnArrayIndex(int x, int y) { return (x) + (CELLCOUNT * (y)); }

/**
 * @brief Translates a cell index into x and y coordinates and returns it.
 *
 * @param CellID
 * @return Vector2i
 */
Vector2i returnCoordinates(int CellID) { return Vector2i(CellID % CELLCOUNT, CellID / CELLCOUNT); }

////////////////GENERAL SETTINGS AND FUNCTIONS///////////////////////////////////

////////////////ENTITY SETTINGS AND FUNCTIONS///////////////////////////////////

/**
 * @brief Base call for entities like players and enemies.
 *
 */
class Entity {
   private:
    // holds the cell index for the entity
    int _cellIndex;
    // renderable body of the entity
    CircleShape _body;

   public:
    /**
     * @brief Construct a new Entity object
     *
     */
    Entity(int x, int y, int cellIndex, Color color, int pointCount) {
        _body.setRadius(CELLSIZE / 4);
        _body.setPosition(Vector2f(x, y));
        _body.setOrigin(Vector2f(_body.getLocalBounds().width / 2, _body.getLocalBounds().height / 2));
        _body.setFillColor(color);
        _body.setPointCount(pointCount);
        SetCellID(cellIndex);
    }

    Entity() : Entity(0, 0, 0, Color::Yellow, 6) {
    }

    /**
     * @brief Returns the body of the Entity
     *
     * @return CircleShape
     */
    CircleShape Body() { return _body; }

    /**
     * @brief Checks for collision between two entities.
     *
     * @param Body
     * @return true
     * @return false
     */
    bool IsAt(FloatRect Body) { return _body.getGlobalBounds().intersects(Body); }

    /**
     * @brief Abstract method that is required in the child classes
     *
     * @return CircleShape
     */
    virtual CircleShape ReturnEntity() = 0;

    /**
     * @brief Set the Cell ID or index of the entity
     *
     * @param ID
     */
    void SetCellID(int ID) { _cellIndex = ID; }

    /**
     * @brief Get the Cell ID or index of the entity
     *
     * @return int
     */
    int GetCellID() { return _cellIndex; }

    /**
     * @brief Updates the position of the entity
     *
     * @param position
     */
    void SetPosition(Vector2f position) {
        _body.setPosition(position);
    }

    /**
     * @brief Updates the color of the entity
     *
     * @param color
     */
    void SetFillColor(Color color) {
        _body.setFillColor(color);
    }

    /**
     * @brief Updates the point count of the entity
     *
     * @param pointCount
     */
    void SetPointCount(int pointCount) {
        _body.setPointCount(pointCount);
    }
};

/**
 * @brief Child of Entity class and builds upon it to allow for the game to have players
 *
 */
class Player : public Entity {
   private:
   public:
    /**
     * @brief Construct a new Player object
     *
     */
    Player() : Entity(0, 0, 0, Color::Yellow, 10) {
    }

    /**
     * @brief Construct a new Player object
     *
     * @param x
     * @param y
     * @param cellIndex
     * @param Color
     */
    Player(int x, int y, int cellIndex, Color color) : Entity(x, y, cellIndex, color, 10) {
    }

    /**
     * @brief Updates the position of the player entity
     *
     * @param position
     */
    void UpdatePosition(Vector2f position) {
        SetPosition(position);
    }

    /**
     * @brief Moves the player to the cell index north of its current index
     *
     * @param neighbours
     * @return int
     */
    int MoveUp(Int4 neighbours) {
        if (neighbours.Top_present) {
            SetCellID(neighbours.Top);
        }
        return GetCellID();
    }

    /**
     * @brief Moves the player to the cell index south of its current index
     *
     * @param neighbours
     * @return int
     */
    int MoveDown(Int4 neighbours) {
        if (neighbours.Bottom_present) {
            SetCellID(neighbours.Bottom);
        }
        return GetCellID();
    }

    /**
     * @brief Moves the player to the cell index west of its current index
     *
     * @param neighbours
     * @return int
     */
    int MoveLeft(Int4 neighbours) {
        if (neighbours.Left_present) {
            SetCellID(neighbours.Left);
        }
        return GetCellID();
    }

    /**
     * @brief Moves the player to the cell index east of its current index
     *
     * @param neighbours
     * @return int
     */
    int MoveRight(Int4 neighbours) {
        if (neighbours.Right_present) {
            SetCellID(neighbours.Right);
        }
        return GetCellID();
    }

    /**
     * @brief Returns the renderable body of the player
     *
     * @return CircleShape
     */
    CircleShape ReturnEntity() override { return Body(); }
};

/**
 * @brief Child of Entity class and builds upon it to allow for the game to have enemies
 *
 */
class Enemy : public Entity {
   private:
    int _timer;         // Waits 3 cycles before next move | Empathy for player
    bool _entityState;  // True - Attack | False - Passive

    /**
     * @brief Switches the state of the enemy from attack to passive or vice versa
     *
     */
    void switchState() { _entityState = !_entityState; }

    /**
     * @brief Checks the state of the current or potential cell index to see if it's the start or end and returns true only if it's a path
     *
     * @param State
     * @return true
     * @return false
     */
    bool CanEnemyBeHere(int State) { return (State == 0) ? true : false; }

    /**
     * @brief Checks if the player is in this particular cell and makes sure it's not the starting cell or the ending cell
     *
     * @param index
     * @param playerIndex
     * @param State
     * @return true
     * @return false
     */
    bool IsPlayerThere(int index, int playerIndex) {
        return (playerIndex == index && !((playerIndex == 0) || (playerIndex == ((CELLCOUNT * CELLCOUNT) - 1)))) ? true : false;
    }

   public:
    /**
     * @brief Construct a new Enemy object
     *
     */
    Enemy() : Enemy(0, 0, 6, 0, Color::Red, true) {
    }

    /**
     * @brief Construct a new Enemy object
     * 
     * @param x 
     * @param y 
     * @param pointCount 
     * @param cellIndex 
     * @param color 
     * @param enemyState 
     */
    Enemy(int x, int y, int pointCount, int cellIndex, Color color, bool enemyState) : Entity(x, y, cellIndex, color, pointCount) {
        _entityState = enemyState;
    }

    /**
     * @brief Resets the enemy with new values
     *
     * @param x
     * @param y
     * @param pointCount
     * @param cellIndex
     * @param color
     */
    void EnemySetup(int x, int y, int pointCount, int cellIndex, Color color, bool EnemyState) {
        SetPosition(Vector2f(x, y));
        SetFillColor(color);
        SetPointCount(pointCount);
        _entityState = EnemyState;
    }

    /**
     * @brief Get the Entity State of the object
     *
     * @return true
     * @return false
     */
    bool GetEntityState() { return _entityState; }

    /**
     * @brief Sets the position to a new value
     *
     * @param position
     */
    void UpdatePosition(Vector2f position) {
        SetPosition(position);
    }

    /**
     * @brief Switches the enemy state and updates the color of the body to reflect this change Red - Attack | Green - Passive
     *
     */
    void SwitchEntityState() {
        if (_entityState) {
            SetFillColor(Color::Red);
        } else {
            SetFillColor(Color::Green);
        }
        switchState();
    }

    /**
     * @brief Moves the enemy to a random neighbour cell by creating a list of potential positions and then shuffling it after which fainally selecting a random one
     *
     * @param neighbours
     * @param playerIndex
     * @param neighbourCellStates
     * @return int
     */
    int MoveEnemy(Int4 neighbours, int playerIndex, Bool4 neighbourCellStates) {
        int i = 0;
        int List[4];
        if (neighbours.Top_present) {
            if (IsPlayerThere(neighbours.Top, playerIndex)) {
                return neighbours.Top;
            } else {
                List[i] = neighbours.Top;
                i++;
            }
        }
        if (neighbours.Right_present) {
            if (IsPlayerThere(neighbours.Right, playerIndex)) {
                return neighbours.Right;
            } else {
                List[i] = neighbours.Right;
                i++;
            }
        }
        if (neighbours.Bottom_present) {
            if (IsPlayerThere(neighbours.Bottom, playerIndex)) {
                return neighbours.Bottom;
            } else {
                List[i] = neighbours.Bottom;
                i++;
            }
        }
        if (neighbours.Left_present) {
            if (IsPlayerThere(neighbours.Left, playerIndex)) {
                return neighbours.Left;
            } else {
                List[i] = neighbours.Left;
                i++;
            }
        }

        random_shuffle(&List[0], &List[i]);
        SetCellID(List[0]);
        return List[0];
    }

    /**
     * @brief Returns the entity body
     * 
     * @return CircleShape 
     */
    CircleShape ReturnEntity() override { return Body(); }
};

////////////////ENTITY SETTINGS AND FUNCTIONS///////////////////////////////////

////////////////GRID SETTINGS AND FUNCTIONS///////////////////////////////////

/**
 * @brief Stack class maintains the stack used in the backtracking algorithm for generating the maze
 *
 */
class Stack {
   private:
    /**
     * @brief struct of a stack which holds a integer value and the pointer to the next node
     *
     */
    typedef struct _stackStruct {
        int value;
        struct _stackStruct *Next;
    } stack;

    typedef stack *StackPtr;  // Pointer of a struct

    StackPtr _stack;  // main stack of the object

    /**
     * @brief Allocates memory and then sets an integer value then updates the stack to point to this new node to the first node in the stack
     *
     * @param Stack
     * @param value
     */
    void Push(StackPtr *Stack, int value) {
        StackPtr NewNode = (stack *)malloc(sizeof(stack));
        NewNode->value = value;
        NewNode->Next = *Stack;
        *Stack = NewNode;
    }

    /**
     * @brief Pulls the value from the first node then frees the memory allocated to it and then updates the stack
     *
     * @param Stack
     * @return int
     */
    int Pop(StackPtr *Stack) {
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
    /**
     * @brief Construct a new Stack object
     *
     */
    Stack() {
        _stack = NULL;
    }

    /**
     * @brief Accepts a integer value and then calls the push function to push a new value into the stack
     *
     * @param value
     */
    void Push(int value) {
        Push(&_stack, value);
    }

    /**
     * @brief Pulls the last integer value from the stack by calling the pop function
     *
     * @return int
     */
    int Pop() {
        return Pop(&_stack);
    }
};

/**
 * @brief Object that holds the data of the walls for a cell
 *
 */
class Wall {
   public:
    /**
     * @brief struct that holds the configuration of the walls in the this cell
     *
     */
    struct WALLS {
        bool TOP;     // 0
        bool RIGHT;   // 1
        bool BOTTOM;  // 2
        bool LEFT;    // 3
    } wall_configuration;

    RectangleShape Walls[4];  // Array holding the renderable bodies of the walls
};

/**
 * @brief Class for a single cell object
 *
 */
class Cell {
   private:
    Wall _cellWalls;       // Walls of the cells
    RectangleShape _cell;  // Renderable body of the cell

    /**
     * @brief Enumeration of the various cell events
     *
     */
    enum CELLEVENTS {
        PATH,
        START,
        END
    } _state;

    /**
     * @brief Function that is run during construction of the object that generates the cells walls
     *
     * @param x
     * @param y
     * @param width
     * @param height
     */
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

    /**
     * @brief Updates the visual look of the walls depending on the walls configuration
     *
     */
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

    /**
     * @brief Updates the cell state visually depending on whether it's a path, start or end
     *
     */
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
    bool visited;  // vairable that holds whether this cell has been visited or not, used during the maze generation process

    /**
     * @brief Construct a new Cell object
     *
     */
    Cell() {
        _state = PATH;
        visited = false;
        _cell.setFillColor(Color::Black);
        _cell.setSize(Vector2f(CELLSIZE, CELLSIZE));
        _cell.setPosition(Vector2f(0, 0));
        GenerateWalls(0, 0, CELLSIZE, CELLSIZE);
    }

    /**
     * @brief Construct a new Cell object
     *
     * @param x
     * @param y
     * @param height
     * @param width
     * @param color
     */
    Cell(int x, int y, int height, int width, Color color) {
        _state = PATH;
        visited = false;
        _cell.setFillColor(color);
        _cell.setSize(Vector2f(CELLSIZE - 3, CELLSIZE - 3));
        _cell.setPosition(Vector2f(x, y));
        GenerateWalls(x, y, CELLSIZE - 3, CELLSIZE - 3);
    }

    /**
     * @brief Get the Wall Configuration object
     *
     * @return Bool4
     */
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

    /**
     * @brief Get the Cell State object
     *
     * @return int
     */
    int GetCellState() {
        return _state;
    }

    /**
     * @brief Set the State object
     *
     * @param ID
     */
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

    /**
     * @brief Returns the positional values or coordinates of the center of the cells body
     *
     * @return Vector2f
     */
    Vector2f ReturnCenter() {
        return Vector2f(_cell.getGlobalBounds().left + _cell.getGlobalBounds().width / 2, _cell.getGlobalBounds().top + _cell.getGlobalBounds().height / 2);
    }

    /**
     * @brief Removes a specific wall and updates the cells walls
     *
     * @param ID
     */
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

    /**
     * @brief Resets the cells values to reflect the new values as set in the parameters
     *
     * @param x
     * @param y
     * @param height
     * @param width
     * @param color
     */
    void ResetCell(int x, int y, int height, int width, Color color) {
        visited = false;
        _cell.setFillColor(color);
        _cell.setSize(Vector2f(CELLSIZE - 3, CELLSIZE - 3));
        _cell.setPosition(Vector2f(x, y));
        GenerateWalls(x, y, CELLSIZE - 3, CELLSIZE - 3);
    }

    /**
     * @brief Gets the renderable component of the cell
     *
     * @return RectangleShape
     */
    RectangleShape CELL() {
        return _cell;
    }

    /**
     * @brief Returns a part of renderable component a wall of the cell
     *
     * @param index
     * @return RectangleShape
     */
    RectangleShape WALL(int index) {
        return _cellWalls.Walls[index];
    }
};

/**
 * @brief Grid object that holds the grid of cells
 *
 */
class Grid {
   private:
    Stack _visitedCells;  // Stack of visited cells used during the maze generation to go back to last visited cell

    /**
     * @brief Returns the configuration of the neighbours present for a particular cell
     *
     * @param Array
     * @param CELL_ID
     * @return Int4
     */
    Int4 noOfNeighbour(Cell Array[], int CELL_ID) {
        Int4 neighbours;
        Vector2i Coordinates = returnCoordinates(CELL_ID);
        if (Coordinates.y - 1 >= 0 && !(Array[returnArrayIndex(Coordinates.x, Coordinates.y - 1)].visited)) {
            neighbours.Top_present = true;
            neighbours.Top = returnArrayIndex(Coordinates.x, Coordinates.y - 1);
        }
        if (Coordinates.y + 1 < CELLCOUNT && !(Array[returnArrayIndex(Coordinates.x, Coordinates.y + 1)].visited)) {
            neighbours.Bottom_present = true;
            neighbours.Bottom = returnArrayIndex(Coordinates.x, Coordinates.y + 1);
        }
        if (Coordinates.x - 1 >= 0 && !(Array[returnArrayIndex(Coordinates.x - 1, Coordinates.y)].visited)) {
            neighbours.Left_present = true;
            neighbours.Left = returnArrayIndex(Coordinates.x - 1, Coordinates.y);
        }
        if (Coordinates.x + 1 < CELLCOUNT && !(Array[returnArrayIndex(Coordinates.x + 1, Coordinates.y)].visited)) {
            neighbours.Right_present = true;
            neighbours.Right = returnArrayIndex(Coordinates.x + 1, Coordinates.y);
        }

        return neighbours;
    }

    /**
     * @brief picks a random neighbour from a collection of neighbours and stores the neighbours cell index and path details and returns it
     *
     * @param NeighboursList
     * @return Vector3i
     */
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

    /**
     * @brief runs a backtracking maze generation algorithm to create a maze in a 2D grid
     *
     */
    void GenerateMaze() {
        srand(time(NULL));
        int No_Visited_Cells = CELLCOUNT * CELLCOUNT;
        int Previous_Cell_ID = returnArrayIndex(rand() % CELLCOUNT, rand() % CELLCOUNT);

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
    Cell grid[CELLCOUNT * CELLCOUNT];  // Main grid of the maze

    /**
     * @brief Construct a new Grid object
     *
     */
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

    /**
     * @brief Get the Neighbours from a cells index
     *
     * @param Index
     * @return Int4
     */
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

    /**
     * @brief Get the Cell States of the neighbouring cells
     *
     * @param Index
     * @return Bool4
     */
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

    /**
     * @brief Returns the renderable body of a cell given its index
     *
     * @param Index
     * @return RectangleShape
     */
    RectangleShape ReturnCell(int Index) {
        return grid[Index].CELL();
    }

    /**
     * @brief Returns the renderable body of a cells walls given its index
     *
     * @param cellIndex
     * @param wallIndex
     * @return RectangleShape
     */
    RectangleShape ReturnWall(int cellIndex, int wallIndex) {
        return grid[cellIndex].WALL(wallIndex);
    }
};

////////////////GRID SETTINGS AND FUNCTIONS///////////////////////////////////

////////////////GUI SETTINGS AND FUNCTIONS///////////////////////////////////

/**
 * @brief UI text object
 *
 */
class UIText {
   private:
    Text _textObj;       // Renderable text
    string _text;        // Holds the string or sentence that is to be rendered
    Color _color;        // Holds the color of the rendered text
    Font _font;          // Holds the font of the rendered text
    int _characterSize;  // Holds the font size
    int _x;              // Holds the x coordinate of the text
    int _y;              // Holds the y coordinate of the text

    /**
     * @brief Sets the values into the renderable text object
     *
     */
    void UpdateText() {
        _textObj.setString(_text);
        _textObj.setPosition(Vector2f(_x, _y));
        _textObj.setFont(_font);
        _textObj.setFillColor(_color);
        _textObj.setCharacterSize(_characterSize);
        _textObj.setOrigin(_textObj.getLocalBounds().left + _textObj.getLocalBounds().width / 2, _textObj.getLocalBounds().top + _textObj.getLocalBounds().height / 2);
    }

   public:
    /**
     * @brief Construct a new UIText object
     *
     * @param x
     * @param y
     * @param characterSize
     * @param text
     * @param color
     */
    UIText(int x, int y, int characterSize, string text, Color color) {
        _x = x;
        _y = y;
        _characterSize = characterSize;
        _text = text;
        _color = color;
        if (!_font.loadFromFile("Projects/Resources/Fonts/PlayfairDisplay-Bold.ttf")) {
            cout << "Failed to open font file" << endl;
        }
    }

    /**
     * @brief Returns the renderable text object
     *
     * @return Text
     */
    Text returnText() {
        UpdateText();
        return _textObj;
    }
};

/**
 * @brief Class UIButton
 *
 */
class UIButton {
   private:
    RectangleShape _button;  // Holds the renderable button object
    Text _buttonText;        // Holds the text of the button
    Font _font;              // Holds the font of the text

    /**
     * @brief Returns the center coordinates of the button
     *
     * @return Vector2f
     */
    Vector2f returnCenter() {
        return Vector2f(_button.getGlobalBounds().left + _button.getGlobalBounds().width / 2, _button.getGlobalBounds().top + _button.getGlobalBounds().height / 2);
    }

   public:
    /**
     * @brief Construct a new UIButton object
     *
     * @param x
     * @param y
     * @param height
     * @param width
     * @param color
     * @param text
     */
    UIButton(int x, int y, int height, int width, Color color, string text) {
        _button.setFillColor(color);
        _button.setPosition(Vector2f(x, y));
        _button.setSize(Vector2f(width, height));
        _button.setOutlineColor(Color::White);
        _button.setOutlineThickness(2);
        _button.setOrigin(_button.getLocalBounds().left + _button.getLocalBounds().width / 2, _button.getLocalBounds().top + _button.getLocalBounds().height / 2);
        _buttonText.setFillColor(Color::White);
        _buttonText.setPosition(returnCenter());
        _buttonText.setString(text);
        _buttonText.setCharacterSize(20);
        if (!_font.loadFromFile("Projects/Resources/Fonts/PlayfairDisplay-Bold.ttf")) {
            cout << "Failed to open font file" << endl;
        }
        _buttonText.setFont(_font);
        _buttonText.setOrigin(_buttonText.getLocalBounds().left + _buttonText.getLocalBounds().width / 2, _buttonText.getLocalBounds().top + _buttonText.getLocalBounds().height / 2);
    }

    /**
     * @brief Set the Outline Color object
     *
     * @param color
     */
    void SetOutlineColor(Color color) {
        _button.setOutlineColor(color);
    }

    /**
     * @brief Set the Font Color object
     *
     * @param color
     */
    void SetFontColor(Color color) {
        _buttonText.setFillColor(color);
    }

    /**
     * @brief Checks if collision was detected
     *
     * @param x
     * @param y
     * @return true
     * @return false
     */
    bool IsAt(float x, float y) {
        return _button.getGlobalBounds().contains(x, y);
    }

    /**
     * @brief Get the Button object
     *
     * @return RectangleShape
     */
    RectangleShape GetButton() {
        return _button;
    }

    /**
     * @brief Get the Button Text object
     *
     * @return Text
     */
    Text GetButtonText() {
        return _buttonText;
    }
};

////////////////GUI SETTINGS AND FUNCTIONS///////////////////////////////////

/**
 * @brief Renders the main menu
 *
 * @return int
 */
int RenderUIWindow() {
    RenderWindow menuWindow(VideoMode(600, 800), "Maze Game", Style::Close | Style::Titlebar);
    int ReturnID = 0;

    Image icon;
    if (!icon.loadFromFile("Projects/Resources/Images/Icon.png")) {
        cout << "Failed to load icon file" << endl;
    }

    menuWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    Texture BackGroundTexture;
    if (!BackGroundTexture.loadFromFile("Projects/Resources/Images/MazeBG.png")) {
        cout << "Can't load BG texture" << endl;
    }
    Sprite BackGround;
    BackGround.setTexture(BackGroundTexture);

    UIText title(299, 100, 85, "Maze Game", Color::White);
    UIText author(500, 785, 20, "By: Tharin Sandipa", Color::White);
    UIText tutorial(299, 350, 20, "How to Play: \n\n>Use the arrow keys to \nnavigate the maze and avoid enemies\n(hint enemies state changes every 2 seconds\n and you can pass through them when they are green).\n\n>Press Q or the Escape key at anytime \nto quit out of the game\n\n>The goal of the game is to go\n from the red block to the green block", Color::White);

    UIButton menuButton(299, 600, 70, 200, Color::Black, "Play Game");

    Event event;
    while (menuWindow.isOpen()) {
        while (menuWindow.pollEvent(event)) {
            switch (event.type) {
                case event.Closed:
                    menuWindow.close();
                    break;
                case Event::KeyPressed:
                    // Press Q or Esq to exit program
                    if (Keyboard::isKeyPressed(Keyboard::Q) || Keyboard::isKeyPressed(Keyboard::Escape)) {
                        menuWindow.close();
                    }
                    break;
            }
        }

        if (menuButton.IsAt(Mouse::getPosition(menuWindow).x, Mouse::getPosition(menuWindow).y)) {
            menuButton.SetOutlineColor(Color::Red);
            menuButton.SetFontColor(Color::Red);
        } else {
            menuButton.SetOutlineColor(Color::White);
            menuButton.SetFontColor(Color::White);
        }

        if (Mouse::isButtonPressed(Mouse::Left)) {
            for (int i = 0; i < 3; i++) {
                if (menuButton.IsAt(Mouse::getPosition(menuWindow).x, Mouse::getPosition(menuWindow).y)) {
                    return 0;
                }
            }
        }

        menuWindow.clear();
        menuWindow.draw(BackGround);
        menuWindow.draw(title.returnText());
        menuWindow.draw(author.returnText());
        menuWindow.draw(tutorial.returnText());
        menuWindow.draw(menuButton.GetButton());
        menuWindow.draw(menuButton.GetButtonText());
        menuWindow.display();
    }
    return 3;
}

/**
 * @brief Renders the game window
 *
 * @return int
 */
int RenderGameWindow() {
    RenderWindow mainWindow(VideoMode(WINDOWDIMENSIONS, WINDOWDIMENSIONS), "Maze Game", Style::Titlebar | Style::Close);
    Event gameEvents;

    Image icon;
    if (!icon.loadFromFile("Projects/Resources/Images/Icon.png")) {
        cout << "Failed to load icon file" << endl;
    }

    mainWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Grid
    Grid Maze;

    // Entities
    Player playerCharacter(Maze.grid[0].ReturnCenter().x, Maze.grid[0].ReturnCenter().y, 0, Color::Yellow);

    vector<Enemy> enemyCharacters;
    for (int i = 0; i < 4; i++) {
        int randomID = 32 + rand() % ((CELLCOUNT * CELLCOUNT) - 32);
        if (randomID <= 10 || randomID == ((CELLCOUNT * CELLCOUNT) - 1)) {
            randomID = 32 + rand() % ((CELLCOUNT * CELLCOUNT) - 32);
        }
        enemyCharacters.push_back(Enemy(Maze.grid[randomID].ReturnCenter().x, Maze.grid[randomID].ReturnCenter().x, 4, randomID, Color::Red, true));
    }

    Clock timer;
    Time time = timer.getElapsedTime();
    Time timeToElaspe = milliseconds(2000);

    while (mainWindow.isOpen()) {
        time = timer.getElapsedTime();
        while (mainWindow.pollEvent(gameEvents)) {
            switch (gameEvents.type) {
                case Event::Closed:
                    mainWindow.close();
                    break;
                case Event::KeyPressed:
                    if (Keyboard::isKeyPressed(Keyboard::Up)) {
                        playerCharacter.UpdatePosition(Maze.grid[playerCharacter.MoveUp(Maze.GetNeighbours(playerCharacter.GetCellID()))].ReturnCenter());
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Down)) {
                        playerCharacter.UpdatePosition(Maze.grid[playerCharacter.MoveDown(Maze.GetNeighbours(playerCharacter.GetCellID()))].ReturnCenter());
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Left)) {
                        playerCharacter.UpdatePosition(Maze.grid[playerCharacter.MoveLeft(Maze.GetNeighbours(playerCharacter.GetCellID()))].ReturnCenter());
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Right)) {
                        playerCharacter.UpdatePosition(Maze.grid[playerCharacter.MoveRight(Maze.GetNeighbours(playerCharacter.GetCellID()))].ReturnCenter());
                    }
                    // Press Q or Esq to exit program
                    if (Keyboard::isKeyPressed(Keyboard::Q) || Keyboard::isKeyPressed(Keyboard::Escape)) {
                        mainWindow.close();
                    }
                    break;
            }
        }

        // Waits a specified time in milliseconds then moves enemy
        if (time.asMilliseconds() >= timeToElaspe.asMilliseconds()) {
            for (int i = 0; i < 4; i++) {
                enemyCharacters[i].UpdatePosition(Maze.grid[enemyCharacters[i].MoveEnemy(Maze.GetNeighbours(enemyCharacters[i].GetCellID()), playerCharacter.GetCellID(), Maze.GetCellStates(enemyCharacters[i].GetCellID()))].ReturnCenter());
                enemyCharacters[i].SwitchEntityState();
            }
            timer.restart();
        }

        if (HasPlayerWon(Maze.grid[playerCharacter.GetCellID()].GetCellState())) {
            return 2;
        }

        for (int i = 0; i < 4; i++) {
            if (HasPlayerLost(playerCharacter.GetCellID(), enemyCharacters[i].GetCellID(), enemyCharacters[i].GetEntityState())) {
                return 1;
            }
        }

        mainWindow.clear();
        for (int i = 0; i < CELLCOUNT * CELLCOUNT; i++) {
            mainWindow.draw(Maze.ReturnCell(i));
            for (int j = 0; j < 4; j++) {
                mainWindow.draw(Maze.ReturnWall(i, j));
            }
        }
        mainWindow.draw(playerCharacter.ReturnEntity());
        for (int i = 0; i < 4; i++) {
            mainWindow.draw(enemyCharacters[i].ReturnEntity());
        }
        mainWindow.display();
    }
    return 3;
}

/**
 * @brief Renders the fail window
 *
 * @return int
 * */
int RenderFailEndScreen() {
    RenderWindow menuWindow(VideoMode(600, 800), "Maze Game", Style::Close | Style::Titlebar);
    int ReturnID = 0;

    Image icon;
    if (!icon.loadFromFile("Projects/Resources/Images/Icon.png")) {
        cout << "Failed to load icon file" << endl;
    }

    menuWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    Texture BackGroundTexture;
    if (!BackGroundTexture.loadFromFile("Projects/Resources/Images/MazeBG.png")) {
        cout << "Can't load BG texture" << endl;
    }
    Sprite BackGround;
    BackGround.setTexture(BackGroundTexture);

    UIText title(299, 100, 85, "Maze Game", Color::White);
    UIText author(500, 785, 20, "By: Tharin Sandipa", Color::White);
    UIText tutorial(299, 350, 20, "You Lost, you can try\nagain if you'd like.\nOr you can press Q or Escape to\nexit the game.\n\nTip: Try luring the enemies\n remember the player is twice as\nfast as the enemies.\n\nThank you for playing", Color::White);

    UIButton menuButton(299, 600, 70, 200, Color::Black, "Play Again");

    Event event;
    while (menuWindow.isOpen()) {
        while (menuWindow.pollEvent(event)) {
            switch (event.type) {
                case event.Closed:
                    menuWindow.close();
                    break;
                case Event::KeyPressed:
                    // Press Q or Esq to exit program
                    if (Keyboard::isKeyPressed(Keyboard::Q) || Keyboard::isKeyPressed(Keyboard::Escape)) {
                        menuWindow.close();
                    }
                    break;
            }
        }

        if (menuButton.IsAt(Mouse::getPosition(menuWindow).x, Mouse::getPosition(menuWindow).y)) {
            menuButton.SetOutlineColor(Color::Red);
            menuButton.SetFontColor(Color::Red);
        } else {
            menuButton.SetOutlineColor(Color::White);
            menuButton.SetFontColor(Color::White);
        }

        if (Mouse::isButtonPressed(Mouse::Left)) {
            for (int i = 0; i < 3; i++) {
                if (menuButton.IsAt(Mouse::getPosition(menuWindow).x, Mouse::getPosition(menuWindow).y)) {
                    return 0;
                }
            }
        }

        menuWindow.clear();
        menuWindow.draw(BackGround);
        menuWindow.draw(title.returnText());
        menuWindow.draw(author.returnText());
        menuWindow.draw(tutorial.returnText());
        menuWindow.draw(menuButton.GetButton());
        menuWindow.draw(menuButton.GetButtonText());
        menuWindow.display();
    }
    return 3;
}

/**
 * @brief Renders the win window
 *
 * @return int
 */
int RenderWinEndScreen() {
    RenderWindow menuWindow(VideoMode(600, 800), "Maze Game", Style::Close | Style::Titlebar);
    int ReturnID = 0;

    Image icon;
    if (!icon.loadFromFile("Projects/Resources/Images/Icon.png")) {
        cout << "Failed to load icon file" << endl;
    }

    menuWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    Texture BackGroundTexture;
    if (!BackGroundTexture.loadFromFile("Projects/Resources/Images/MazeBG.png")) {
        cout << "Can't load BG texture" << endl;
    }
    Sprite BackGround;
    BackGround.setTexture(BackGroundTexture);

    UIText title(299, 100, 85, "Maze Game", Color::White);
    UIText author(500, 785, 20, "By: Tharin Sandipa", Color::White);
    UIText tutorial(299, 350, 20, "You Won !!!\n\n\nIf you'd like you can play the game again.\n Or you can press Q or Escape to\nexit the game.\n Thank you for playing", Color::White);

    UIButton menuButton(299, 600, 70, 200, Color::Black, "Play Again");

    Event event;
    while (menuWindow.isOpen()) {
        while (menuWindow.pollEvent(event)) {
            switch (event.type) {
                case event.Closed:
                    menuWindow.close();
                    break;
                case Event::KeyPressed:
                    // Press Q or Esq to exit program
                    if (Keyboard::isKeyPressed(Keyboard::Q) || Keyboard::isKeyPressed(Keyboard::Escape)) {
                        menuWindow.close();
                    }
                    break;
            }
        }

        if (menuButton.IsAt(Mouse::getPosition(menuWindow).x, Mouse::getPosition(menuWindow).y)) {
            menuButton.SetOutlineColor(Color::Red);
            menuButton.SetFontColor(Color::Red);
        } else {
            menuButton.SetOutlineColor(Color::White);
            menuButton.SetFontColor(Color::White);
        }

        if (Mouse::isButtonPressed(Mouse::Left)) {
            for (int i = 0; i < 3; i++) {
                if (menuButton.IsAt(Mouse::getPosition(menuWindow).x, Mouse::getPosition(menuWindow).y)) {
                    return 0;
                }
            }
        }

        menuWindow.clear();
        menuWindow.draw(BackGround);
        menuWindow.draw(title.returnText());
        menuWindow.draw(author.returnText());
        menuWindow.draw(tutorial.returnText());
        menuWindow.draw(menuButton.GetButton());
        menuWindow.draw(menuButton.GetButtonText());
        menuWindow.display();
    }
    return 3;
}

/**
 * @brief Checks which screen to render and renders it
 *
 * @return int
 */
int main() {
    int i = RenderUIWindow();
    while (i != 3) {
        switch (i) {
            case 0:
                i = RenderGameWindow();
                break;
            case 1:
                i = RenderFailEndScreen();
                break;
            case 2:
                i = RenderWinEndScreen();
                break;
            case 3:
                return 0;
        }
    }
    return 0;
}