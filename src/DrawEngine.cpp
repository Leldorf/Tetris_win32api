#include "DrawEngine.h"

DrawEngine::DrawEngine(HDC hdc, HWND hwnd, int pxPerBlock,
	int width, int height) :
	hdc(hdc), hwnd(hwnd), width(width), height(height)
{
	GetClientRect(hwnd, &rect);

	SaveDC(hdc);

	// Set up coordinate system
	SetMapMode(hdc, MM_ISOTROPIC);
	SetViewportExtEx(hdc, pxPerBlock, pxPerBlock, 0);
	SetWindowExtEx(hdc, 1, -1, 0);
	SetViewportOrgEx(hdc, 0, rect.bottom, 0);

	// Set default colors
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkColor(hdc, RGB(70, 70, 70));
	SetBkMode(hdc, TRANSPARENT);
}

DrawEngine::~DrawEngine()
{
	// Restore coordinate system
	RestoreDC(hdc, -1);
}

void DrawEngine::drawBlock(int x, int y, COLORREF color)
{
	HBRUSH hBrush = CreateSolidBrush(color);
	rect.left = x;
	rect.right = x + 1;
	rect.top = y;
	rect.bottom = y + 1;

	FillRect(hdc, &rect, hBrush);

	// Draw left and bottom black border
	MoveToEx(hdc, x, y + 1, NULL);
	LineTo(hdc, x, y);
	LineTo(hdc, x + 1, y);
	DeleteObject(hBrush);
}

void DrawEngine::drawInterface()
{
	// Draw a gray area at the right
	HBRUSH hBrush = CreateSolidBrush(RGB(200, 200, 200));
	
	rect.top = height;
	rect.left = width;
	rect.bottom = 0;
	rect.right = width + 6;
	FillRect(hdc, &rect, hBrush);
	
	rect.top = 0;
	rect.bottom = -3;
	rect.left = 0;
	FillRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);

	hBrush = CreateSolidBrush(RGB(0,0,0));
	
	rect.top = height - 1;
	rect.bottom = height - 5;
	rect.left = width + 1;
	rect.right = width + 5;
	FillRect(hdc, &rect, hBrush);

	//SetBkMode(hdc, OPAQUE);
	drawText(TEXT("Keep"), width+1, 20);
	drawText(TEXT("Next"), width+1, 15);
	//SetBkMode(hdc, TRANSPARENT);
	DeleteObject(hBrush);
}

void DrawEngine::drawText(TCHAR* szText, int x, int y) const
{
	TextOut(hdc, x, y, szText, lstrlen(szText));
}

void DrawEngine::drawTitle()
{
	HFONT hfont = CreateFont(5, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("°íµñ"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hfont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 0, 0));
	TextOut(hdc, 11, 15, TEXT("T"), 1);
	SetTextColor(hdc, RGB(0, 255, 0));
	TextOut(hdc, 16, 15, TEXT("E"), 1);
	SetTextColor(hdc, RGB(255, 127, 0));
	TextOut(hdc, 21, 15, TEXT("T"), 1);
	SetTextColor(hdc, RGB(255, 0, 255));
	TextOut(hdc, 26, 15, TEXT("R"), 1);
	SetTextColor(hdc, RGB(64, 64, 255));
	TextOut(hdc, 31, 15, TEXT("I"), 1);
	SetTextColor(hdc, RGB(255, 255, 0));
	TextOut(hdc, 36, 15, TEXT("S"), 1);
	SetBkColor(hdc, RGB(255, 255, 255));
	SetTextColor(hdc, RGB(0, 0, 0));
	oldFont = (HFONT)SelectObject(hdc, oldFont);
	//SetBkMode(hdc, OPAQUE);
	TextOut(hdc, 24, 3, TEXT("START!"), 6);
	TextOut(hdc, 24, 2, TEXT("BATTLE"), 6);
	TextOut(hdc, 24, 1, TEXT(" EXIT "), 6);
	//SetBkMode(hdc, TRANSPARENT);
}

void DrawEngine::drawAbout(TCHAR* about, int data, int x, int y) const
{
	TCHAR szBuffer[20];
	int len = wsprintf(szBuffer, TEXT("%s: %5d"), about, data);
	SetBkColor(hdc,RGB(255, 255, 255));
	//SetBkMode(hdc, OPAQUE);
	TextOut(hdc, x, y, szBuffer, len);
	//SetBkMode(hdc, TRANSPARENT);
}

void DrawEngine::drawNextPiece(Piece& piece, int x, int y)
{
	COLORREF color = piece.getColor();

	// Draw the piece in a 4x4 square area
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (piece.isPointExists(i, j))
			{
				drawBlock(i + x, j + y, color);

			}
			else
			{
				drawBlock(i + x, j + y, RGB(0, 0, 0));

			}
		}
	}
}

void DrawEngine::drawMino(Piece& piece, int x, int y, COLORREF color)
{
	POINT apt[4];
	piece.getBody(apt);
	// Draw the piece in a 4x4 square area
	for (int i = 0; i < 4; i++)
	{
		if (apt[i].y + y < 20)
			drawBlock(apt[i].x + x, apt[i].y + y, color);
	}
}

void DrawEngine::DrawMenuSelect(int n)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 230, 200));
	rect.left = 22;
	rect.right = 24;
	rect.top = 0;
	rect.bottom = 3;
	FillRect(hdc, &rect, hBrush);
	SetTextColor(hdc, RGB(0, 0, 0));
	for (int i = 0; i < 3; i++)
	{
		if (i == n)
			TextOut(hdc, 22, 3 - i, TEXT("->"), 2);

	}
}