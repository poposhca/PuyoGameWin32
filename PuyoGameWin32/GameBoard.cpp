#include "GameBoard.h"
#include <stdexcept>
#include "GameConstants.h"

GameBoard::GameBoard()
{
	this->Board = new std::unordered_map<int, std::unordered_map<int, Square*>>();
}

//Add a new Square to the HashMap
void GameBoard::addToBoard(Square *square)
{
	if (!square)
		throw std::invalid_argument("Null square argument");

	//Create and insert a new HashMap
	if (this->Board->count(square->GetY()) == 0)
	{
		std::unordered_map<int, Square*> newMap;
		newMap.insert({ square->GetX(), square });
		this->Board->insert({ square->GetY(), newMap });
	}
	//Insert the square
	auto row = Board->find(square->GetY());
	row->second.insert({ square->GetX(), square });
}

//Delete a square from the HashMap
void GameBoard::deleteFromBoard(Square *square)
{
	if (!square)
		throw std::invalid_argument("Null square argument");

	if (this->Board->count(square->GetY()) == 0 || !square)
		return;
	auto row = Board->find(square->GetY());
	row->second.erase(square->GetX());
	if (row->second.empty())
		this->Board->erase(square->GetY());
}

//Return the number of squares of the same color that are connected in the hashmap
int GameBoard::LookForEq(Square *square)
{
	if (!square)
		throw std::invalid_argument("Null square argument");

	int count;
	auto hash = new std::unordered_map<Square*, bool>();
	count = LookForEq(square->GetY(), square->GetX(), square->GetType(), square->GetSize(), hash);
	hash->clear();
	delete hash;
	return count;
}

//Return the number of squares of the same color that are connected in the hashmap
int GameBoard::LookForEq(int row, int column, SquareType type, int size, std::unordered_map<Square*, bool> *hash)
{
	if (!hash)
		throw std::invalid_argument("Null hash map");

	int count = 0;
	if (this->KeysExists(row, column))
	{
		auto r = this->Board->find(row);
		auto s = r->second.find(column)->second;
		if (s->GetType() == type && hash->count(s) == 0)
		{
			hash->insert({ s,true });
			count = 1;
			count += LookForEq(row - size, column - size, type, size, hash);	//TopLeft
			count += LookForEq(row - size, column + size, type, size, hash);	//TopRight
			count += LookForEq(row, column - size, type, size, hash);			//Left
			count += LookForEq(row, column + size, type, size, hash);			//Right
			count += LookForEq(row + size, column - size, type, size, hash);	//BottomLeft
			count += LookForEq(row + size, column, type, size, hash);			//Bottom
			count += LookForEq(row + size, column + size, type, size, hash);	//BottomRight
		}
	}
	return count;
}

//Change the state of squares of the same color that are connected to destroy
void GameBoard::MarkToDelete(Square * square)
{
	if (!square)
		throw std::invalid_argument("Square Null");
	
	auto hash = new std::unordered_map<Square*, bool>();
	MarkToDelete(square->GetY(), square->GetX(), square->GetType(), square->GetSize(), hash);
	hash->clear();
	delete hash;
}

//Change the state of squares of the same color that are connected to destroy
void GameBoard::MarkToDelete(int row, int column, SquareType type, int size, std::unordered_map<Square*, bool> *hash)
{
	if (!hash)
		throw std::invalid_argument("HashMap Null");

	if (this->KeysExists(row, column))
	{
		auto r = this->Board->find(row);
		auto s = r->second.find(column)->second;
		if (s->GetType() == type && hash->count(s) == 0)
		{
			hash->insert({ s,true });
			s->state = SquareState::destroy;
			MarkToDelete(row - size, column - size, type, size, hash);	//TopLeft
			MarkToDelete(row - size, column + size, type, size, hash);	//TopRight
			MarkToDelete(row, column - size, type, size, hash);			//Left
			MarkToDelete(row, column + size, type, size, hash);			//Right
			MarkToDelete(row + size, column - size, type, size, hash);	//BottomLeft
			MarkToDelete(row + size, column, type, size, hash);			//Bottom
			MarkToDelete(row + size, column + size, type, size, hash);	//BottomRight
		}
	}
}

//Returns true if keys exists in the gameboard
bool GameBoard::KeysExists(int row, int column)
{
	if (this->Board->count(row) > 0)
	{
		auto RowMap = this->Board->find(row);
		if (RowMap->second.count(column))
			return true;
	}
	return false;
}

//Remove from board and from memory all marked items
//A square that is removed may have neighbours above, those must be moved down
//Those neightbors are added to a list of lists called toMove
//The list is handled by "Game.cpp"
void GameBoard::DeleteMarked(std::vector<std::vector<Square*>*> *toMove)
{
	if (!toMove)
		throw std::invalid_argument("List Null");

	//Delete all sqaures marked as "delete"
	std::vector<Square*> DelVector;
	for (auto row = this->Board->begin(); row != this->Board->end(); row++)
	{
		for (auto column = row->second.begin(); column != row->second.end(); column++)
		{
			std::vector<Square*> *list = new std::vector<Square*>();
			auto square = column->second;
			if (square->state == SquareState::destroy)
			{
				DelVector.push_back(column->second);
				LookForEqUp(square->GetY() - square->GetSize(), square->GetX(), list);
			}
			if (!list->empty())
				toMove->push_back(list);
		}
	}
	for (auto item = DelVector.begin(); item != DelVector.end(); item++)
	{
		deleteFromBoard(*item);
		delete *item;
	}

	for (auto i = toMove->begin(); i != toMove->end(); i++)
	{
		for (auto j = (*i)->begin(); j != (*i)->end(); j++)
		{
			deleteFromBoard(*j);
		}
	}
}

//Look for squares that are above of a deleted square
void GameBoard::LookForEqUp(int row, int column, std::vector<Square*> *toMove)
{
	if (!toMove)
		throw std::invalid_argument("List Null");

	if (this->Board->count(row) == 1)
	{
		auto hashRow = this->Board->find(row)->second;
		if (hashRow.count(column) == 1)
		{
			auto square = this->Board->find(row)->second.find(column)->second;
			if (square->state != SquareState::destroy)
			{
				toMove->push_back(square);
				LookForEqUp(row - square->GetSize(), column, toMove);
			}
		}
	}
}

//Delete all pinters from the HashMap
void GameBoard::deleteAll()
{
	for (auto i = this->Board->begin(); i != this->Board->end(); i++)
	{
		auto map = i->second;
		for (auto j = map.begin(); j != map.end(); j++)
		{
			delete j->second;
		}
	}
	this->Board->clear();
}

GameBoard::~GameBoard()
{
	deleteAll();
	delete this->Board;
}
