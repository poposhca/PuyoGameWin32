#include "Game.h"
#include <stdlib.h>

Game::Game(int SquareSize = 20, int SpawnX = 0, int MaxX = 10, int MaxY = 10, int MoveSpeed = 20)
{
	this->squares = new std::vector<std::vector<Square*>*>();
	this->SquareSize = SquareSize;
	this->MoveSpeed = MoveSpeed;
	this->SpawnX = SpawnX;
	this->MaxX = MaxX - SquareSize;
	this->MaxY = MaxY - (SquareSize * 2);
	this->state = StartGame;
}

//Detects if a square will collide with the floor or with other square
//Also says if a square is at the top level
CollisionPosition& Game::DetectCollision(Square *sq, GameBoard *board)
{
	CollisionPosition collision;
	int actx = sq->GetX();
	int acty = sq->GetY();
	
	//Collision at Up, it means Game over
	if (acty == 0)
		collision.Up = true;
	//Collision at bottom
	if (acty + this->SquareSize >= this->MaxY || board->KeysExists(acty + this->SquareSize, actx))
		collision.Down = true;
	//Collision at right
	if (actx + this->SquareSize >= this->MaxX || board->KeysExists(acty, actx + this->SquareSize))
		collision.Rigt = true;
	//Colision at left
	if (actx - this->SquareSize <= 0 || board->KeysExists(acty, actx - this->SquareSize))
		collision.Left = true;
	return collision;
}

//CREATES RANDOM SQUARES AND THEY ARE ASSIGN TO S1 AND S2
//S1 and S2 represents the squares that are controlled by the player
//S1 can be moved around s2 clockwise or counterclockwise
//Both can ve move to the left or to the right
void Game::CreateSquares()
{
	if (s1)
	{
		if (s1->state == SquareState::destroy)
			delete s1;
	}
	if (s2)
	{
		if (s2->state == SquareState::destroy)
			delete s2;
	}
	int type;
	type = rand() % 4;
	this->s1 = new Square(this->SpawnX, 0,this->SquareSize, type, SquareState::playeble);
	type = rand() % 4;
	this->s2 = new Square(this->SpawnX + this->SquareSize, 0, this->SquareSize, type, SquareState::playeble);
}

//Moves playable squares S1 and S2 one position down every game cycle
//They are two important states: when both are playeble, and when a square hits something and the other keeps falling
//without control.
//After S1 and S2 hits something, they are added to the gameboard and new random squares are created
void Game::moveMainSquares(GameBoard *board)
{
	if (this->s1 && this->s2 && board)
	{
		//Both squares can be moved by the player
		if (this->s1->state == SquareState::playeble && this->s2->state == SquareState::playeble)
		{
			CollisionPosition cpS1 = this->DetectCollision(this->s1, board);
			CollisionPosition cpS2 = this->DetectCollision(this->s2, board);

			//Game Over condition
			if ((cpS1.Down && cpS1.Up) || (cpS2.Down && cpS2.Up))
				this->state = GameState::GamneOver;

			//Both squares hits the ground or other squares
			if (((cpS1.Down || cpS2.Down) && (this->s1->GetX() == this->s2->GetX())) || (cpS1.Down && cpS2.Down))
			{
				board->addToBoard(this->s1);
				board->addToBoard(this->s2);
				if (board->LookForEq(this->s1) >= 4)
				{
					board->MarkToDelete(this->s1);
					board->MarkToDelete(this->s2);
				}
				else
				{
					this->s1->state = SquareState::fixed;
					this->s2->state = SquareState::fixed;
				}
			}
			//S1 hits the ground or othe square and S2 must keep falling
			else if (cpS1.Down && this->s1->GetY() == this->s2->GetY())
			{
				this->s2->state = SquareState::unplayable;
				board->addToBoard(this->s1);
				if (board->LookForEq(this->s1) >= 4) 
					board->MarkToDelete(this->s1);
				else
					this->s1->state = SquareState::fixed;
			}
			//S2 hits the ground or othe square and S1 must keep falling
			else if (cpS2.Down && this->s1->GetY() == this->s2->GetY())
			{
				this->s1->state = SquareState::unplayable;
				board->addToBoard(this->s2);
				if (board->LookForEq(this->s2) >= 4)
					board->MarkToDelete(this->s2);
				else
					this->s2->state = SquareState::fixed;
			}
			else
			{
				this->s1->SetY(this->s1->GetY() + this->MoveSpeed);
				this->s2->SetY(this->s2->GetY() + this->MoveSpeed);
			}
		}
		//S1 keeps falling, but is not controlled
		else if (this->s1->state == SquareState::unplayable)
		{
			CollisionPosition cpS1 = this->DetectCollision(this->s1, board);
			if (cpS1.Down)
			{
				board->addToBoard(this->s1);
				if (board->LookForEq(this->s1) >= 4)
					board->MarkToDelete(this->s1);
				else
					this->s1->state = SquareState::fixed;
			}
			else
				this->s1->SetY(this->s1->GetY() + this->MoveSpeed);
		}
		//S2 keeps falling, but is not controlled
		else if (this->s2->state == SquareState::unplayable)
		{
			CollisionPosition cpS2 = this->DetectCollision(this->s2, board);
			if (cpS2.Down)
			{
				board->addToBoard(this->s2);
				if (board->LookForEq(this->s2) >= 4)
					board->MarkToDelete(this->s2);
				else
					this->s2->state = SquareState::fixed;
			}
			else
				this->s2->SetY(this->s2->GetY() + this->MoveSpeed);
		}
		//
		if ((this->s1->state == SquareState::fixed || this->s1->state == SquareState::destroy)
			&& (this->s2->state == SquareState::fixed || this->s2->state == SquareState::destroy))
		{
			board->DeleteMarked(squares);
			CreateSquares();
		}
	}
	else
	{
		CreateSquares();
	}
}

//Move squares that falls after deletion of an other squares.
//These squares are added into a list of lists (squares) by a GameBoard object after deleting a square.
//If the first square of each list hits something, the complete list is added to the gameboard and deleted from squares
void Game::moveOtherSquares(GameBoard *board)
{
	int pos = 0;
	std::vector<int> delvec;
	if (!this->squares->empty())
	{
		//Move the square one position down
		for (auto list = this->squares->begin(); list != this->squares->end(); list++)
		{
			Square *fall = (*list)->at(0);

			//If collision then add to the gameboard and delete it from the vector
			if (DetectCollision(fall, board).Down)
			{
				delvec.push_back(pos);
				for (auto i = (*list)->begin(); i != (*list)->end(); i++)
				{
					board->addToBoard(*i);
				}
			}
			else
			{
				for (auto i = (*list)->begin(); i != (*list)->end(); i++)
				{
					Square *s = *i;
					s->SetY(s->GetY() + s->GetSize());
				}
			}
			pos++;
		}
		//Delete the once added to the board
		for (auto i = delvec.rbegin(); i != delvec.rend(); i++)
		{
			this->squares->erase(this->squares->begin() + *i);
		}
	}
}

//Moves and rotate S1 and S2 every tipe a valid user input is made.
void Game::moveSquare(Direction dir, GameBoard *board)
{
	CollisionPosition cS1 = DetectCollision(this->s1, board);
	CollisionPosition cS2 = DetectCollision(this->s2, board);
	
	//Move the Square to the right
	if (dir == Direction::MoveLeft)
	{
		if (!cS1.Left && !cS2.Left)
		{
			this->s1->SetX(this->s1->GetX() - this->SquareSize);
			this->s2->SetX(this->s2->GetX() - this->SquareSize);
		}
	}

	//Move the Square to the right
	if (dir == Direction::MoveRight)
	{
		if (!cS1.Rigt && !cS2.Rigt)
		{
			this->s1->SetX(this->s1->GetX() + this->SquareSize);
			this->s2->SetX(this->s2->GetX() + this->SquareSize);
		}
	}

	//Rotates the Square
	if (dir == Direction::RotateLeft || dir == Direction::RotateRight)
	{
		//Variable that tells where to rotate the square:
		//0 = up, 1=right, 2=down, 3=left
		byte rotate;

		//Clockwise direction
		if (dir == Direction::RotateRight)
		{
			//Move up or down
			if (this->s1->GetY() == this->s2->GetY())
			{
				if (this->s1->GetX() < this->s2->GetX())
					rotate = 0;
				else
					rotate = 2;
			}
			//Move left or right
			else
			{
				if (this->s1->GetY() < this->s2->GetY())
					rotate = 3;
				else
					rotate = 1;
			}
		}
		//Non-clockwise
		else if (dir == Direction::RotateLeft)
		{
			//Move up or down
			if (this->s1->GetY() == this->s2->GetY())
			{
				if (this->s1->GetX() < this->s2->GetX())
					rotate = 2;
				else
					rotate = 0;
			}
			//Move left or right
			else
			{
				if (this->s1->GetY() < this->s2->GetY())
					rotate = 1;
				else
					rotate = 3;
			}
		}

		switch (rotate)
		{
			//UP
		case 0:
			this->s1->SetX(this->s2->GetX());
			this->s1->SetY(this->s2->GetY() + this->SquareSize);
			break;
			//RIGHT
		case 1:
			this->s1->SetX(this->s2->GetX() + this->SquareSize);
			this->s1->SetY(this->s2->GetY());
			break;
			//DOWN
		case 2:
			this->s1->SetX(this->s2->GetX());
			this->s1->SetY(this->s2->GetY() - this->SquareSize);
			break;
			//LEFT
		case 3:
			this->s1->SetX(this->s2->GetX() - this->SquareSize);
			this->s1->SetY(this->s2->GetY());
			break;
		}
	}

}

//Moves any other square one position down
//Called by the squares in the vector
void Game::FallSquare(Square *square, GameBoard *board)
{
	CollisionPosition cpS = DetectCollision(square, board);
	if (cpS.Down)
		square->state = SquareState::fixed;
	else
		square->SetY(square->GetY() + this->SquareSize);
}

Game::~Game()
{
	delete s1;
	delete s2;
	delete squares;
}
