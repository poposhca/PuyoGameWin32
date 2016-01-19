#pragma once
#include <vector>
#include "Square.h"
#include "GameBoard.h"
#include "GameStates.h"

//Class that updates all squares in the game and mantains the cycle

class Game
{
private:
	int SquareSize;
	int MoveSpeed;
	int SpawnX;
	int MaxX;
	int MaxY;

public:
	std::vector<std::vector<Square*>*> *squares;
	Square *s1;
	Square *s2;
	GameState state;

	Game(int SquareSize, int SpawnX, int MaxX, int MaxY, int MoveSpeed);
	void CreateSquares();
	void moveMainSquares(GameBoard *board);
	void moveOtherSquares(GameBoard *board);
	void moveSquare(Direction dir, GameBoard *board);
	void FallSquare(Square *square, GameBoard *board);
	CollisionPosition& Game::DetectCollision(Square *sq, GameBoard *board);
	~Game();
};

