#include <iostream>
#include <conio.h>
#include <windows.h>
using namespace std;

int snake_x, snake_y, fruit_x, fruit_y;
const int width = 20;
const int height = 20;
bool gameOver;
int score;
enum direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
direction dir;
int tail_x[100], tail_y[100];
int ntail = 0;

void SetUp()
{
	gameOver = false;
	snake_x = width / 2;
	snake_y = height / 2;
	fruit_x = rand() % width;
	fruit_y = rand() % height;
	score = 0;
	dir = STOP;
}

void Draw()
{
	system("cls");
	for (int i = 0; i < width; i++)
	{
		cout << "#";
	}
	cout << endl;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++) 
		{
			if (snake_x == j && snake_y == i)
			{
				cout << "O";
				continue;
			}
			else if (fruit_x == j && fruit_y == i)
			{
				cout << "F";
				continue;
			}
			else if (j == 0 || j == width - 1)
			{
				cout << "#";
			}
			else
			{
				bool print = false;
				for (int k = 0; k < ntail; k++)
				{
					
					if (tail_x[k] == j && tail_y[k] == i)
					{
						cout << "o";
						print = true;
					}
					
				}
				if (!print)
				{
					cout << " ";
				}
				
			}
		}
		cout << endl;
	}
	for (int i = 0; i < width; i++)
	{
		cout << "#";
	}
	cout << endl;
	cout << "Score: " << score << endl;
}

void Input()
{
	if (_kbhit())
	{
		switch (_getch())
		{
		case 'a':
			dir = LEFT;
			break;
		case 'd':
			dir = RIGHT;
			break;
		case 'w':
			dir = UP;
			break;
		case 's':
			dir = DOWN;
			break;
		case 'x':
			gameOver = true;
			break;
			
		}
	}
}

void Logic()
{
	if (snake_x == fruit_x && snake_y == fruit_y)
	{
		score += 10;
		fruit_x = rand() % width;
		fruit_y = rand() % height;
		ntail++;
	}
	
	int prex = tail_x[0];
	int prey = tail_y[0];
	
	int pre2x, pre2y;
	tail_x[0] = snake_x;
	tail_y[0] = snake_y;
	
	for (int i = 1; i < ntail; i++)
	{
		pre2x = tail_x[i];
		pre2y = tail_y[i];
		tail_x[i] = prex;
		tail_y[i] = prey;
		prex = pre2x;
		prey = pre2y;
	}
	
	
	switch (dir)
	{
	case LEFT:
		snake_x--;
		break;
	case RIGHT:
		snake_x++;
		break;
	case UP:
		snake_y--;
		break;
	case DOWN:
		snake_y++;
		break;
	default:
		break;
	}
	
	if (snake_x < 0 || snake_x >= width || snake_y < 0 || snake_y >= height)
	{
		gameOver = true;
	}
	
	
	
}

int main()
{
	SetUp();
	while (!gameOver)
	{
		
		Input();
		Logic();
		Draw();
		Sleep(1000);
	}
	
}


