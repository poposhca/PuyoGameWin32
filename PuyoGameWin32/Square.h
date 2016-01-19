#pragma once
#include <Windows.h>
#include <mutex>
#include "GameStates.h"
//
class Square
{
private:
	std::mutex m;
	int x;
	int y;
	int size;
	COLORREF rgb;
	int r, g, b;
	SquareType type;
	void SetRGB();
	void SetType(int type);
public:
	SquareState state;

	Square(int x, int y, int size, int type);
	Square(int x, int y, int size, int type, SquareState state);
	void SetX(int x);
	void SetY(int y);
	int GetX();
	int GetY();
	int GetSize();
	RECT *GetSquareRECT();
	SquareType GetType();
	COLORREF GetRGB();
	int GetR();
	int GetG();
	int GetB();
	~Square();
};

