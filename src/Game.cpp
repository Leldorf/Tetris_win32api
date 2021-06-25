#include "Game.h"
#include <iostream>

using namespace std;

Game::Game(DrawEngine& de) :
	de(de), isPaused(false), titleSelect(0)
{
	level = new Level(de, 10, 20);
	gamemode = TITLE;
	gameExit = false;
}

Game::~Game()
{
	delete level;
}

void Game::restart()
{
	delete level;
	level = new Level(de, 10, 20);
	isPaused = false;
	repaint();
}

bool Game::keyPress(int vk)
{
	if (gamemode == TITLE)
	{
		switch (vk)
		{
		case VK_RETURN:
			switch (titleSelect)
			{
			case 0:
				gamemode = GAME1P;
				de.drawInterface();
				return true;
			case 1:
				return false;
			case 2:
				gameExit = true;
				return true;
			default:
				return false;
			}
		case VK_UP:
			titleSelect = (2 + titleSelect) % 3;
			break;
		case VK_DOWN:
			titleSelect = (4 + titleSelect) % 3;
			break;
		}
		return false;
	}
	// When pausing, ignore keys other than PAUSE and ENTER
	if (vk != VK_ESCAPE && vk != VK_RETURN && isPaused)
		return false;

	switch (vk)
	{
	case 'x':
	case 'X':
		level->rotate(true);
		break;
	case 'z':
	case 'Z':
		level->rotate(false);
		break;
	case 'c':
	case 'C':
		level->holdChange();
		break;
	case 'v':
	case 'V':
		level->addRows();
		break;
	case VK_SPACE:
		level->hardDrop();
		break;
	case VK_DOWN:
		level->move(0, -1);
		break;
	case VK_LEFT:
		level->move(-1, 0);
		break;
	case VK_RIGHT:
		level->move(1, 0);
		break;
	case VK_PAUSE:
		pause(!isPaused);
		break;
	case VK_RETURN:
		// You can only restart on game over
		if (level->isGameOver())
			restart();
		break;
	case VK_ESCAPE:
		if (isPaused)
			pause(false);
		else
			pause(true);
		break;
	default:
		return false;
	}
	return true;
}

void Game::timerUpdate()
{
	if (gamemode == TITLE)
	{
		de.drawTitle();
		de.DrawMenuSelect(titleSelect);
		return;
	}
	// Don't update game when pausing
	if (isPaused)
		return;

	// If game is over, show GAME OVER
	if (level->isGameOver()) {
		isPaused = true;
		drawGameOver();
		return;
	}

	// Update game data
	level->timerUpdate();

}

void Game::pause(bool paused)
{
	// Don't pause if game is over
	if (isGameOver() || gamemode == TITLE)
		return;

	isPaused = paused;
	if (paused)
		drawPause();
	level->drawState();
}

void Game::repaint() const
{
	if (gamemode == TITLE)
	{
		de.drawTitle();
		return;
	}
	de.drawInterface();
	level->drawState();
	level->drawNextPiece();
	level->drawBoard();
	if (level->isGameOver())
		drawGameOver();
	else if (isPaused)
		drawPause();
}

bool Game::isGameOver() const
{
	return level->isGameOver();
}
