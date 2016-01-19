#include "Square.h"
#include "GameConstants.h"

Square::Square(int x, int y, int size, int type)
{
	this->x = x;
	this->y = y;
	this->size = size;
	SetType(type);
	SetRGB();
	this->state = SquareState::unplayable;
}

Square::Square(int x, int y, int size, int type, SquareState state)
{
	this->x = x;
	this->y = y;
	this->size = size;
	this->state = state;
	SetType(type);
	SetRGB();
}

//Sets x and y coordinates threathsafe
void Square::SetX(int x)
{
	m.lock();
	this->x = x;
	m.unlock();
}

void Square::SetY(int y)
{
	m.lock();
	this->y = y;
	m.unlock();
}

//Gets y
int Square::GetX()
{
	return x;
}

int Square::GetY()
{
	return y;
}

int Square::GetSize()
{
	return this->size;
}

//Sets the RGB values
void Square::SetRGB()
{
	switch (type)
	{
	case red:
		this->rgb = RGB(255, 0, 0);
		break;
	case green:
		this->rgb = RGB(0, 255, 0);
		break;
	case blue:
		this->rgb = RGB(0, 81, 255);
		break;
	case yellow:
		this->rgb = RGB(246, 255, 0);
		break;
	}
}

void Square::SetType(int type)
{
	switch (type)
	{
	case 0:
		this->type = SquareType::red;
		break;
	case 1:
		this->type = SquareType::yellow;
		break;
	case 2:
		this->type = SquareType::green;
		break;
	case 3:
		this->type = SquareType::blue;
		break;
	}
}

//Returns a pointer to the a RECT struct
RECT *Square::GetSquareRECT()
{
	RECT *rect = new RECT;
	rect->left = x;
	rect->top = y;
	rect->right = size + x;
	rect->bottom = size + y;
	return rect;
}

//Getters for RGB values
COLORREF Square::GetRGB()
{
	return rgb;
}
int Square::GetR() { return r; }
int Square::GetG() { return g; }
int Square::GetB() { return b; }

//Getter for the Sqaure type
SquareType Square::GetType()
{
	return this->type;
}

Square::~Square()
{
#ifdef DEBUG
	wchar_t buffer[100];
	wsprintf(buffer, L"Destruido: %p\n", this);
	OutputDebugString(buffer);
#endif // DEBUG
}
