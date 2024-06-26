#pragma once
#include "General.hpp"

class Entity {
   private:
    int _cellIndex;  // specifies which cell the entity is in
    CircleShape _body;

   public:
    Entity() {
        _body.setRadius(CELLSIZE / 4);
        _body.setPosition(Vector2f(0, 0));
        _body.setOrigin(Vector2f(_body.getLocalBounds().width / 2, _body.getLocalBounds().height / 2));
        _body.setFillColor(Color::Yellow);
    }

    bool IsAt(FloatRect Body) { return _body.getGlobalBounds().intersects(Body); }

    CircleShape ReturnEntity() { return _body; }

    void SetCellID(int ID) { _cellIndex = ID; }

    int GetCellID() { return _cellIndex; }
};

class Player : public Entity {
   private:
    CircleShape _body;

   public:
    Player() {
        _body.setRadius(CELLSIZE / 4);
        _body.setPosition(Vector2f(0, 0));
        _body.setOrigin(Vector2f(_body.getLocalBounds().width / 2, _body.getLocalBounds().height / 2));
        _body.setFillColor(Color::Yellow);
        SetCellID(0);
    }

    Player(int x, int y, int cellIndex, Color color) {
        _body.setRadius(CELLSIZE / 4);
        _body.setPosition(Vector2f(x, y));
        _body.setOrigin(Vector2f(_body.getLocalBounds().width / 2, _body.getLocalBounds().height / 2));
        _body.setFillColor(color);
        SetCellID(cellIndex);
    }

    void UpdatePosition(Vector2f position) {
        _body.setPosition(position);
    }

    int MoveUp(Int4 neighbours) {
        if (neighbours.Top_present) {
            SetCellID(neighbours.Top);
        }
        return GetCellID();
    }

    int Move(int i) {
        SetCellID(i);
        return GetCellID();
    }

    int MoveDown(Int4 neighbours) {
        if (neighbours.Bottom_present) {
            SetCellID(neighbours.Bottom);
        }
        return GetCellID();
    }

    int MoveLeft(Int4 neighbours) {
        if (neighbours.Left_present) {
            SetCellID(neighbours.Left);
        }
        return GetCellID();
    }
    int MoveRight(Int4 neighbours) {
        if (neighbours.Right_present) {
            SetCellID(neighbours.Right);
        }
        return GetCellID();
    }

    CircleShape ReturnPlayer() { return _body; }
};

class Enemy : public Entity {
   private:
    // Waits 3 cycles before next move | Empathy for player
    int _timer;
    CircleShape _body;
    bool _entityState;  // True - Attack | False - Passive

    void switchState() { _entityState = !_entityState; }

    bool CanEnemyBeHere(int State) { return (State == 0) ? true : false; }

    bool IsPlayerThere(int index, int playerIndex, int State) {
        return (playerIndex == index && !((playerIndex == 0) || (playerIndex == ((CELLCOUNT * CELLCOUNT) - 1)))) ? true : false;
    }

   public:
    Enemy() {
        _body.setRadius(CELLSIZE / 4);
        _body.setPointCount(6);
        _body.setPosition(Vector2f(0, 0));
        _body.setOrigin(Vector2f(_body.getLocalBounds().width / 2, _body.getLocalBounds().height / 2));
        _body.setFillColor(Color::Red);
        SetCellID(0);
    }

    Enemy(int x, int y, int pointCount, int cellIndex, Color color) {
        _body.setRadius(CELLSIZE / 4);
        _body.setPointCount(pointCount);
        _body.setPosition(Vector2f(x, y));
        _body.setOrigin(Vector2f(_body.getLocalBounds().width / 2, _body.getLocalBounds().height / 2));
        _body.setFillColor(color);
        SetCellID(cellIndex);
    }

    void EnemySetup(int x, int y, int pointCount, int cellIndex, Color color) {
        _body.setRadius(CELLSIZE / 4);
        _body.setPointCount(pointCount);
        _body.setPosition(Vector2f(x, y));
        _body.setOrigin(Vector2f(_body.getLocalBounds().width / 2, _body.getLocalBounds().height / 2));
        _body.setFillColor(color);
        SetCellID(cellIndex);
    }

    bool GetEntityState() { return _entityState; }

    void UpdatePosition(Vector2f position) {
        _body.setPosition(position);
    }

    void SwitchEntityState() {
        if (_entityState) {
            _body.setFillColor(Color::Red);
        } else {
            _body.setFillColor(Color::Green);
        }
        switchState();
    }

    int MoveEnemy(Int4 neighbours, int playerIndex, Bool4 neighbourCellStates) {
        int i = 0;
        int List[4];
        if (neighbours.Top_present) {
            if (IsPlayerThere(neighbours.Top, playerIndex, neighbourCellStates.x)) {
                return neighbours.Top;
            } else {
                List[i] = neighbours.Top;
                i++;
            }
        }
        if (neighbours.Right_present) {
            if (IsPlayerThere(neighbours.Right, playerIndex, neighbourCellStates.y)) {
                return neighbours.Right;
            } else {
                List[i] = neighbours.Right;
                i++;
            }
        }
        if (neighbours.Bottom_present) {
            if (IsPlayerThere(neighbours.Bottom, playerIndex, neighbourCellStates.z)) {
                return neighbours.Bottom;
            } else {
                List[i] = neighbours.Bottom;
                i++;
            }
        }
        if (neighbours.Left_present) {
            if (IsPlayerThere(neighbours.Left, playerIndex, neighbourCellStates.w)) {
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

    CircleShape ReturnEnemy() { return _body; }
};
