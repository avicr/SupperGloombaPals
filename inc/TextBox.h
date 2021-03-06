#pragma once

#include "Globals.h"

enum eTextBoxStyle
{
	TBS_Modern,
	TBS_Zelda
};

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
	const static int FontSizeInPixels = 32;
	const static int Margin = 16;
	int PosX;
	int PosY;
	int Width;
	int Height;
	int CharacterFrameDelay;  // How long to wait between frames to write a character
	int FrameCount;
	int CurrentPage;
	int CurrentLine;
	int CurCharIndex;
	bool bDone;
	bool bWaitingForInput;
	int CursorFrame;
	bool bButtonPressedLastFrame;
	bool bNeedToLetGoOfButton;
	bool bPlotDevice;
	eTextBoxStyle Style;

	SDL_Color TextBGColor;
	
	SDL_Texture* Texture;
	eTextBoxState State;
	string Text;
	
	vector< vector <string> > Pages;

public:
	TextBox(int InPosX, int InPosY, int InWidth, int InHeight, string InText, bool bIsPlotDevice = false, eTextBoxStyle InStyle = TBS_Modern);
	~TextBox();
	
	void Tick();
	void Render(SDL_Renderer* Renderer);
	bool IsDone();
	eTextBoxStyle GetStyle();
};