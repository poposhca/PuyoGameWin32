#pragma once
#include <unordered_map>
#include <vector>
#include "Square.h"

//Contains the squares in memory using a Hash table
class GameBoard
{
public:
	GameBoard();
	std::unordered_map<int, std::unordered_map<int, Square*>> *Board;
	void addToBoard(Square *square);
	void deleteFromBoard(Square *square);
	int LookForEq(Square *square);
	int LookForEq(int row, int column, SquareType type, int size, std::unordered_map<Square*, bool> *hash);
	void MarkToDelete(Square *square);
	void MarkToDelete(int row, int column, SquareType type, int size, std::unordered_map<Square*, bool> *hash);
	bool KeysExists(int row, int column);
	void DeleteMarked(std::vector<std::vector<Square*>*> *toMove);
	void LookForEqUp(int row, int column, std::vector<Square*> *toMove);
	void deleteAll();
	~GameBoard();
};

