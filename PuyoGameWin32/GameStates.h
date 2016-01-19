#pragma once

//Contains enumerations that describes states for the squares and for the game itself.

//Enum for describe the main game state
enum GameState
{
	StartGame,	//The game is about to start
	InGame,		//The game cycle is active
	PauseGame,	//The game cycle is active, but with no updates to Game object
	GamneOver,	//The player fails, but it can be restarted
	EndGame		//The window is going to be closed
};

//Tells where the square is going to move
enum Direction
{
	MoveRight,		//Move S1 and S2 to the right
	MoveLeft,		//Move S1 and S2 to the left
	RotateRight,	//Rotates S1 clockwise
	RotateLeft		//Rotates S2 counterclockwise
};

//Enum for describe the state of each square in the game
enum SquareState
{
	playeble,		//The player can interact with the square
	unplayable,		//The player can't controll the square but keeps falling
	fall,			//The square falls faster
	fixed,			//The square can't move
	destroy			//The square is going to be destroyed
};

//Defines the color of the Square
enum SquareType
{
	red,
	yellow,
	green,
	blue
};

//Position where a collision is detected
struct CollisionPosition
{
	bool Up = false;
	bool Down = false;
	bool Rigt = false;
	bool Left = false;
};