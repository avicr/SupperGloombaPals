#pragma once

#include "Globals.h"

enum eTextBoxState
{
	TS_GROWING = 0,
	TS_WRITING,
	TS_WAITING,
	TS_SCROLLING,
	TS_SHRINKING
};

class TextBox 
{
protected:
	int PosX;
	int PosY;
	int WriteSpeed;
	int FrameCount;
	eTextBoxState State;
	string ThingToSay;

public:
	TextBox(string Text);
	
	void Tick();
	void Render(SDL_Renderer* Renderer);
};