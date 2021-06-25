#include "Level.h"
#include <ctime>

Level::Level(DrawEngine& de, int width, int height) :
	de(de), width(width), height(height), lastTime(0.0), speed(500), score(-1)
{
	srand(time(0));

	// Allocate the drawing board
	board = new COLORREF * [width];
	for (int i = 0; i < width; i++)
	{
		board[i] = new COLORREF[height];
		for (int j = 0; j < height; j++)
			board[i][j] = RGB(0, 0, 0);
	}
	topHeight = 0;
	// 初始化目前和下一個方塊
	shadowY = 0;
	hold = NULL;
	isHold = true;
	level = 0;
	nextRow = 15;
	randomPieceBox();
	current = pieceSet.getPiece(sevBag[0]);
	next[0] = pieceSet.getPiece(sevBag[1]);
	next[1] = pieceSet.getPiece(sevBag[2]);
	next[2] = pieceSet.getPiece(sevBag[3]);
	
	nextCnt = 4;

	dropRandomPiece();
	isGameover = false;
	lastTime = timeGetTime();
	drawState();
	drawNextPiece();
}

Level::~Level()
{
	for (int i = 0; i < width; i++)
		delete[] board[i];
	delete[] board;
}

void Level::drawBoard()
{
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			de.drawBlock(i, j, board[i][j]);
	drawCurrent();
}

void Level::timerUpdate()
{
	// If the time isn't up, don't drop nor update
	currentTime = timeGetTime();
	if (currentTime - lastTime < speed)
		return;

	// Time's up, drop
	// If the piece hits the bottom, check if player gets score, drop the next
	// piece, increase speed, redraw info
	// If player gets score, increase more speed
	move(0, -1);
	lastTime = timeGetTime();
}

bool Level::place(int x, int y, const Piece& piece)
{
	// Out of boundary or the position has been filled
	if (x + piece.getWidth() > width || isCovered(piece, x, y))
		return false;

	posX = x;
	posY = y;

	POINT apt[4];
	piece.getBody(apt);

	COLORREF color = piece.getColor();

	for (int i = 0; i < 4; i++)
	{
		if (y + apt[i].y > height - 1)
			continue;
		board[x + apt[i].x][y + apt[i].y] = color;
	}
	//drawCurrent();
	return true;
}

void Level::drawCurrent()
{
	POINT apt[4];
	current->getBody(apt);
	int y;
	for (y = topHeight; y >= 0; y--)
	{
		bool flag = false;
		for (int i = 0; i < 4; i++)
		{
			if (board[posX + apt[i].x][y + apt[i].y] != RGB(0, 0, 0))
			{
				flag = true;
				break;
			}
		}
		if (flag) break;
	}
	shadowY = y+1;
	de.drawMino(*current, posX, shadowY, RGB(50, 50, 50));
	de.drawMino(*current, posX, posY, current->getColor());
}

bool Level::rotate(bool clockwise)
{
	Piece* tmp = current;

	// Move the piece if it needs some space to rotate
	int disX = max(posX + current->getHeight() - width, 0);

	// Go to next rotation state (0-3)
	int rotation;
	if(clockwise)
		rotation = (current->getRotation() + 1) % PieceSet::NUM_ROTATIONS;
	else
		rotation = (current->getRotation() + 3) % PieceSet::NUM_ROTATIONS;


	clear(*current);
	current = pieceSet.getPiece(current->getId(), rotation);

	// Rotate successfully
	if (place(posX - disX, posY, *current))
	{
		drawBoard();
		//drawCurrent();
		return true;
	}

	// If the piece cannot rotate due to insufficient space, undo it
	current = tmp;
	place(posX, posY, *current);
	return false;
}

bool Level::move(int cxDistance, int cyDistance)
{
	if (posX + cxDistance < 0 || posX + current->getWidth() + cxDistance > width)
		return false;
	if (cxDistance < 0 && isHitLeft())
		return false;
	if (cxDistance > 0 && isHitRight())
		return false;
	if (cyDistance < 0 && (posY <= 0 || isHitBottom()))
	{
		clear(*current);
		place(posX, posY, *current);
		POINT apt[4];
		if (current->getHeight() + posY > topHeight)
			topHeight = current->getHeight() + posY;
		
		int lines = clearRows();
		speed = max(speed - 2 * lines, 100);
		score += 1 + lines * lines * 5;
		dropRandomPiece();
		drawState();
		drawNextPiece();
		nextRow--;
		if (nextRow == 0)
		{
			level++;
			nextRow = 15 - level;
			addRows();
		}
		isHold = true;
		lastTime = timeGetTime();
		drawBoard();
		return false;
	}
	clear(*current);
	//place(posX + cxDistance, posY + cyDistance, *current);
	posX = posX + cxDistance;
	posY = posY + cyDistance;
	drawBoard();
}

bool Level::hardDrop()
{
	clear(*current);
	posY = shadowY;
	place(posX, posY, *current);
	POINT apt[4];
	if (current->getHeight() + posY > topHeight)
		topHeight = current->getHeight() + posY;
	int lines = clearRows();
	speed = max(speed - 2 * lines, 100);
	score += 1 + lines * lines * 5;
	dropRandomPiece();
	drawState();
	drawNextPiece();
	nextRow--;
	if (nextRow == 0)
	{
		level++;
		nextRow = 15 - level;
		addRows();
	}
	isHold = true;
	lastTime = timeGetTime();
	drawBoard();
	return false;
}

void Level::clear(const Piece& piece)
{
	POINT apt[4];
	piece.getBody(apt);
	int x, y;
	for (int i = 0; i < 4; i++) {
		x = posX + apt[i].x;
		y = posY + apt[i].y;
		if (x > width - 1 || y > height - 1)
			continue;
		board[posX + apt[i].x][posY + apt[i].y] = RGB(0, 0, 0);
	}
}

void Level::dropRandomPiece()
{
	current = next[0];
	next[0] = next[1];
	next[1] = next[2];
	if (nextCnt == 7)
	{
		nextCnt = 0;
		randomPieceBox();
	}
	next[2] = pieceSet.getPiece(sevBag[nextCnt]);
	nextCnt++;
	place(3, height - 1, *current);
}

void Level::randomPieceBox()
{
	int i;
	for (i = 0; i < 7; i++)
		sevBag[i] = -1;
	for (i = 0; i < 7; i++)
	{
		int n = rand() % 7;
		while (sevBag[n % 7] != -1)
			n++;
		sevBag[n % 7] = i;
	}
}

bool Level::isHitBottom() const
{
	POINT apt[4];
	int n = current->getSkirt(apt);
	int x, y;
	for (int i = 0; i < n; i++)
	{
		x = posX + apt[i].x;
		y = posY + apt[i].y;
		if (y <= 0 || (y == 0 || board[x][y - 1] != RGB(0, 0, 0)))
			return true;
	}
	return false;
}

bool Level::isHitLeft() const
{
	POINT apt[4];
	int n = current->getLeftSide(apt);
	int x, y;
	for (int i = 0; i < n; i++)
	{
		x = posX + apt[i].x;
		y = posY + apt[i].y;
		if (y > height - 1)
			continue;
		if (x == 0 || board[x - 1][y] != RGB(0, 0, 0))
			return true;
	}
	return false;
}

bool Level::isHitRight() const
{
	POINT apt[4];
	int n = current->getRightSide(apt);
	int x, y;
	for (int i = 0; i < n; i++)
	{
		x = posX + apt[i].x;
		y = posY + apt[i].y;
		if (y > height - 1)
			continue;
		if (x == width - 1 || board[x + 1][y] != RGB(0, 0, 0))
			return true;
	}
	return false;
}

bool Level::isCovered(const Piece& piece, int x, int y) const
{
	POINT apt[4];
	piece.getBody(apt);
	int tmpX, tmpY;
	for (int i = 0; i < 4; i++)
	{
		tmpX = apt[i].x + x;
		tmpY = apt[i].y + y;
		if (tmpX > width - 1 || tmpY > height - 1)
			continue;
		if (board[tmpX][tmpY] != RGB(0, 0, 0))
			return true;
	}
	return false;
}

int Level::clearRows()
{
	bool isComplete = true;
	int rows = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (board[j][i] == RGB(0, 0, 0))
			{
				isComplete = false;
				break;
			}
			// The row is full
			if (j == width - 1)
				isComplete = true;
		}
		// If the row is full, clear it (fill with black)
		if (isComplete)
		{
			for (int j = 0; j < width; j++)
				board[j][i] = RGB(0, 0, 0);

			// Move rows down
			for (int k = i; k < height - 1; k++)
			{
				for (int m = 0; m < width; m++)
					board[m][k] = board[m][k + 1];
			}
			i = -1;
			rows++;
		}
	}
	topHeight -= rows;
	return rows;
}

int Level::addRows()
{
	int notBlock = rand()%width;
	for (int i = topHeight; i >= 0; i--)
	{
		for (int j = 0; j < width; j++)
		{
			board[j][i + 1] = board[j][i];
		}
	}
	for (int i = 0; i < width; i++)
	{
		if (notBlock == i)
		{
			board[i][0] = RGB(0, 0, 0);
			continue;
		}
		board[i][0] = RGB(127, 127, 127);
	}
	topHeight++;
	if (topHeight >= height - 1)
	{
		isGameover = true;
	}
	drawState();
	return 0;
}

bool Level::holdChange()
{
	if (isHold)
	{
		if (hold == NULL)
		{
			hold = current;
			clear(*current);
			dropRandomPiece();

		}
		else
		{
			Piece* temp = hold;
			hold = current;
			clear(*current);
			current = temp;
		}
		lastTime = timeGetTime();
		place(3, height - 1, *current);
		isHold = false;
		drawNextPiece();
		drawBoard();
		return true;
	}
	else
		return false;
}

bool Level::isGameOver()
{
	// Exclude the current piece
	if (current)
		clear(*current);

	// If there's a piece on the top, game over
	for (int i = 0; i < width; i++) {
		if (board[i][height - 1]) {
			if (current)
				place(posX, posY, *current);
			return true;
		}
	}

	// Put the current piece back
	if (current != 0)
		place(posX, posY, *current);
	return false;
}

void Level::drawState() const
{
	de.drawAbout(L"speed", (500 - speed) / 1, 1, -1);
	de.drawAbout(L"level", level, 4, -1);
	de.drawAbout(L"next_", nextRow, 7, -1);
	de.drawAbout(L"score", topHeight, 10, -1);
}

void Level::drawNextPiece() const
{
	if(hold != NULL)
		de.drawNextPiece(*hold, width + 1, 15);
	de.drawNextPiece(*next[0], width + 1, 10);
	de.drawNextPiece(*next[1], width + 1, 5);
	de.drawNextPiece(*next[2], width + 1, 0);
}
