#ifndef LEVEL_H
#define LEVEL_H

#include "DrawEngine.h"
#include "PieceSet.h"
#include <windows.h>

//
// Level stores current snapshot of the game, including the dropping piece,
// the next piece, the piece generator (PieceSet), the position of the
// dropping piece, the width and height of the level, the dropping speed,
// and a two-dimentional color array that is used to represent the canvas.
//
class Level
{
public:
	// de: used to draw the level
	// width & height: level size in cells
	Level(DrawEngine& de, int width = 10, int height = 20);
	~Level();

	// Draws the level
	void drawBoard();

	// Rotates the dropping piece, returns true if successful
	bool rotate(bool clockwise);

	// Moves the dropping piece, returns true if successful
	// cxDistance is horizontal movement, positive value is right
	// cyDistance is vertical movement, positive value is up (normally it's
	// negaive)
	bool move(int cxDistance, int cyDistance);
	bool hardDrop();

	// Updates the level based on the current speed
	void timerUpdate();

	bool isGameOver();

	// Draw different kinds of info
	void drawState() const;
	void drawNextPiece() const;
	void drawCurrent();
	bool holdChange();
	int addRows();

protected:
	// Places a piece somewhere
	// If there isn't enough space, does nothing and returns false
	bool place(int x, int y, const Piece& piece);

	// Clears a piece on the canvas
	void clear(const Piece& piece);

	// Releases the next dropping piece
	void dropRandomPiece();
	void randomPieceBox();
	// Checks if the piece hits the boundary
	bool isHitBottom() const;
	bool isHitLeft() const;
	bool isHitRight() const;

	// Checks if a piece can move to a position
	bool isCovered(const Piece& piece, int x, int y) const;

	// Clears "full" rows, returns the number of rows being cleared
	int clearRows();

	COLORREF** board;    // The cavnas, the drawing board[width][height]
	DrawEngine& de;      // Does graphics stuffs
	PieceSet pieceSet;   // Piece generator
	Piece* current;      // Current dropping piece
	Piece* next[3];      // Next piece
	Piece* hold;		 // hold piece
	int sevBag[7];      // Number of next pieces
	int nextCnt;		 // Count of next blocks using
	int width;           // Level width (in cells)
	int height;          // Level height
	int posX;            // X coordinate of dropping piece (Cartesian system)
	int posY;            // Y coordinate of dropping piece
	int shadowY;         // Y coordinate of shadow
	int speed;           // Drop a cell every _speed_ millisecs
	double lastTime;     // Last time updated
	double currentTime;  // Current update time
	int topHeight;		 // Height of top blocks
	int score;           // Player's score
	bool isHold;		
	bool isGameover;
	int level;
	int nextRow;
};

#endif // LEVEL_H
