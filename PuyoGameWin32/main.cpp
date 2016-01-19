#include <Windows.h>
#include <tchar.h>
#include <thread>
//Game Headers
#include "Game.h"
#include "GameBoard.h"
#include "GameConstants.h"

//APLICATION CONSTANT NAMES
static wchar_t CLASSNAME[] = L"PuyoGame";
static wchar_t WINNAME[] = L"MainWindow";

//GLOBAL GAME MANAGER
static Game PuyoGame(SQUARESIZE, SPAWN, WINDOWWITH * SQUARESIZE, WINDOWHEIGHT * SQUARESIZE, SQUARESIZE);
static GameBoard PuyoBoard;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void GameLoop(HWND hWnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) 
{
	//CREATE AND REGISTER WINDOWS CLASS
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = CLASSNAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, L"Fatal error, can't create Windows Class", L"PuyoPuyo Game", MB_OK);
		return 9;
	}

	//CREATE AND SHOW WINDOW
	HWND mainWindow = CreateWindow(CLASSNAME, WINNAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SQUARESIZE * WINDOWWITH, SQUARESIZE * WINDOWHEIGHT, NULL, NULL, hInstance, NULL);
	if (!mainWindow) {
		MessageBox(NULL, L"Can't create Window", L"PuyoPuyo Game", MB_OK);
		return 9;
	}

	//Show game controlls
	wchar_t message[] = L"CONTROLS:\nQ: Rotate clockwise\nW: Rotate counterclockwise\nLeft and Right arrows: Move left and right";
	MessageBox(NULL, message, L"PuyoPuyo Game", MB_OK);
	ShowWindow(mainWindow, 10);

	//CREATEGAME
	PuyoGame.CreateSquares();
	PuyoGame.state = InGame;
	std::thread GameThread(GameLoop, mainWindow);

	//MESSAGE LOOP
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GameThread.join();
	return 0;
}

//GameLoop
//Updates the game every 100ms (Moves all the squares and send the rpaint message to the MESSAGE LOOP)
void GameLoop(HWND hWnd)
{
	while (PuyoGame.state != GameState::EndGame)
	{
		if (PuyoGame.state == GameState::InGame)
		{
			PuyoGame.moveMainSquares(&PuyoBoard);
			PuyoGame.moveOtherSquares(&PuyoBoard);
			RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_ERASE);
			std::this_thread::sleep_for(std::chrono::milliseconds(GAMELOOPSPEED));
		}
		else if (PuyoGame.state == GameState::GamneOver)
		{
			if (MessageBox(hWnd, L"Try again?", L"PuyoPuyo Game", MB_YESNO) == IDYES)
			{
				PuyoGame.state = GameState::InGame;
				PuyoBoard.deleteAll();
				PuyoGame.CreateSquares();
			}
			else
			{
				PuyoGame.state = GameState::EndGame;
			}
		}
	}
}

//Proc Function, reads player inputs
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HDC hdc;
		PAINTSTRUCT ps;

	case WM_KEYDOWN:
		switch (wParam)
		{
			//Left arrow key
		case VK_LEFT:
			if (PuyoGame.s1->state == SquareState::playeble && PuyoGame.s2->state == SquareState::playeble)
				PuyoGame.moveSquare(Direction::MoveLeft, &PuyoBoard);
			break;
			//Right arrow key
		case VK_RIGHT:
			if (PuyoGame.s1->state == SquareState::playeble && PuyoGame.s2->state == SquareState::playeble)
				PuyoGame.moveSquare(Direction::MoveRight, &PuyoBoard);
			break;
			//Q key
		case 0x51:
			if (PuyoGame.s1->state == SquareState::playeble)
				PuyoGame.moveSquare(Direction::RotateLeft, &PuyoBoard);
			break;
			//W key
		case 0x57:
			if (PuyoGame.s1->state == SquareState::playeble)
				PuyoGame.moveSquare(Direction::RotateRight, &PuyoBoard);
			break;
		case VK_ESCAPE:
			PuyoGame.state = GameState::PauseGame;
			if (MessageBox(hWnd, L"Exit game?", L"PuyoPuyo", MB_YESNO) == IDYES)
			{
				PuyoGame.state = GameState::EndGame;
				DestroyWindow(hWnd);
			}
			else
			{
				PuyoGame.state = GameState::InGame;
			}
			break;
		}
		//RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_ERASE);
		break;

	case WM_PAINT:
		if (PuyoGame.state == InGame)
		{
			hdc = BeginPaint(hWnd, &ps);
			//Paint S1 and S2
			FillRect(hdc, PuyoGame.s1->GetSquareRECT(), CreateSolidBrush(PuyoGame.s1->GetRGB()));
			FillRect(hdc, PuyoGame.s2->GetSquareRECT(), CreateSolidBrush(PuyoGame.s2->GetRGB()));
			//Paint other squares
			if (!PuyoGame.squares->empty())
			{
				for (auto v = PuyoGame.squares->begin(); v != PuyoGame.squares->end(); v++)
				{
					for (auto s = (*v)->begin(); s != (*v)->end(); s++)
					{
						FillRect(hdc, (*s)->GetSquareRECT(), CreateSolidBrush((*s)->GetRGB()));
					}
				}
			}
			//Paint the Board
			for (auto row = PuyoBoard.Board->begin(); row != PuyoBoard.Board->end(); row++)
			{
				for (auto cln = row->second.begin(); cln != row->second.end(); cln++)
				{
					Square *tmp = cln->second;
					if (tmp->state == SquareState::destroy)
						FillRect(hdc, tmp->GetSquareRECT(), CreateSolidBrush(RGB(0, 0, 0)));
					if (tmp->state == SquareState::fall)
					{
						PuyoGame.FallSquare(tmp, &PuyoBoard);
						FillRect(hdc, tmp->GetSquareRECT(), CreateSolidBrush(tmp->GetRGB()));
					}
					else
						FillRect(hdc, tmp->GetSquareRECT(), CreateSolidBrush(tmp->GetRGB()));
				}
			}
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_CLOSE:
		PuyoGame.state = GameState::EndGame;
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}