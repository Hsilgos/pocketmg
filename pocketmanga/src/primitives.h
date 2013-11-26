#pragma once


struct Position
{
	int x;
	int y;

	Position();
	Position(int ax, int ay);
	void clean();
};


struct Rect
{
	int x;
	int y;
	int width;
	int height;

	Rect();
	Rect(int ax, int ay, int width, int height);
	void clear();
};

