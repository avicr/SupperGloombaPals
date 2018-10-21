#include "../inc/TextBox.h"
#include "../inc/PlayerSprite.h"
#include "../inc/Game.h"
#include <queue>
#include <sstream>

TextBox::TextBox(int InPosX, int InPosY, int InWidth, int InHeight, string InText, bool bIsPlotDevice)
{
	SDL_assert(InText.length() < 4096);
	
	bPlotDevice = bIsPlotDevice;
	Style = TBS_Modern;

	if (Style == TBS_Zelda)
	{
		TextBGColor = { 255, 0, 255, 0 };
		CharacterFrameDelay = 6;
	}
	else
	{
		TextBGColor = { 0, 0, 0, 255 };
		CharacterFrameDelay = 4;
	}

	PosX = InPosX;
	PosY = InPosY;
	Width = InWidth;
	Height = InHeight;

	Text = InText;

	Texture = SDL_CreateTexture(GRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, InWidth, InHeight);
	
	SDL_SetRenderTarget(GRenderer, Texture);	
	SDL_SetRenderDrawColor(GRenderer, TextBGColor.r, TextBGColor.g, TextBGColor.b, TextBGColor.a);
	SDL_SetTextureBlendMode(Texture, SDL_BLENDMODE_BLEND);
	SDL_RenderClear(GRenderer);
	SDL_SetRenderTarget(GRenderer, NULL);

	char RealText[4096];
	strcpy(RealText, Text.c_str());
	char* TextPtr = RealText;
	int TextLength = strlen(RealText);
	
	int CurLineLen = 0;	
	CurrentLine = 0;
	CurrentPage = 0;

	Pages.push_back(vector<string>());
	Pages[0].push_back("");

	std::queue<std::string> result;
	std::istringstream iss(InText);
	for (std::string s; iss >> s; )
	{
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
		int EscapeCharIndex = s.find("\\");

		if (EscapeCharIndex != -1)
		{
			string EscapeSequenceString = s.substr(EscapeCharIndex, 2);			

			string LeftString = s.substr(0, EscapeCharIndex);
			string RightString = s.substr(EscapeCharIndex+2, s.length() - (EscapeCharIndex+2));

			if (LeftString != "")
			{
				result.push(LeftString);				
			}
			
			result.push(EscapeSequenceString);
			
			if (RightString != "")
			{
				result.push(RightString);
			}
		}
		else
		{
			result.push(s);
		}
	}

	while (result.size() != 0)
	{								
		string Word = result.front();				

		if (Word == "\\n" || Word == "\\N")
		{
			result.pop();
			CurrentLine++;
			CurLineLen = 0;
			Pages[CurrentPage].push_back(string());
		}
		else if (Word == "\\s" || Word == "\\S")
		{
			result.pop();			
			CurLineLen++;
			Pages[CurrentPage][CurrentLine].push_back(' ');
		}
		else if (Word == "\\p" || Word == "\\P")
		{
			result.pop();
			CurLineLen = 0;
			CurrentLine = 0;
			CurrentPage++;
			Pages.push_back(vector<string>());
			Pages[CurrentPage].push_back(string());
		}
		else if (Margin + CurLineLen + Word.length() * FontSizeInPixels < Width)
		{
			Pages[CurrentPage][CurrentLine] += Word + " ";
			CurLineLen += FontSizeInPixels * (Word.length() + 1);
			result.pop();
		}
		else
		{
			CurLineLen = 0;						
			
			if (Margin + (CurrentLine + 2) * FontSizeInPixels >= Height)
			{
				CurrentLine = 0;
				CurrentPage++;
				Pages.push_back(vector<string>());
				Pages[CurrentPage].push_back(string());
			}
			else
			{
				CurrentLine++;
				Pages[CurrentPage].push_back(string());
			}
		}		
	}

	bWaitingForInput = false;
	CurrentPage = 0;
	CurrentLine = 0;
	FrameCount = CharacterFrameDelay;
	CurCharIndex = 0;
	bDone = false;
	CursorFrame = 0;
	bButtonPressedLastFrame = false;
	bNeedToLetGoOfButton = true;
}

void TextBox::Tick()
{	
	
	FrameCount--;
	CursorFrame++;

	if (bDone)
	{
		return;
	}	

	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if (ThePlayer->IsButton1Pressed(state) && !bNeedToLetGoOfButton)
	{
		FrameCount -= 2;
	}

	if (bWaitingForInput && !bButtonPressedLastFrame)
	{		
		if (ThePlayer->IsButton1Pressed(state))
		{
			bWaitingForInput = false;
			bNeedToLetGoOfButton = true;

			CurrentPage++;
			CurrentLine = 0;
			FrameCount = CharacterFrameDelay;
			CurCharIndex = 0;

			SDL_SetRenderTarget(GRenderer, Texture);
			SDL_SetRenderDrawColor(GRenderer, TextBGColor.r, TextBGColor.g, TextBGColor.b, TextBGColor.a);
			SDL_RenderClear(GRenderer);
			SDL_SetRenderTarget(GRenderer, NULL);

			// Did we write the last page in the dialog?
			if (CurrentPage >= Pages.size())
			{
				bDone = true;
			}
		}
		else
		{
			return;
		}
	}
	else if (FrameCount <= 0 && !bWaitingForInput)
	{
		SDL_SetRenderTarget(GRenderer, Texture);
		string CharToRender = Pages[CurrentPage][CurrentLine].substr(CurCharIndex, 1);
		DrawBitmapText(CharToRender, Margin + CurCharIndex * FontSizeInPixels, Margin + CurrentLine * FontSizeInPixels, FontSizeInPixels, FontSizeInPixels, GRenderer, FontShadowedWhite, 1, 1, false);
		FrameCount = CharacterFrameDelay;
		CurCharIndex++;
		Mix_PlayChannel(CHANNEL_TEXT, TextSound, 0);
		// Did we write the last character in the line?
		if (CurCharIndex >= Pages[CurrentPage][CurrentLine].length())
		{				
			// Did we write the last line in the page?
			if (CurrentLine + 1 >= Pages[CurrentPage].size())
			{
				if (Style != TBS_Zelda)
				{
					bWaitingForInput = true;
				}
				else
				{
					CurCharIndex = 0;
					if (CurrentPage == Pages.size() - 1)
					{
						bDone = true;
					}
				}
			}
			else
			{				
				CurCharIndex = 0;
				CurrentLine++;				
			}
		}

		SDL_SetRenderTarget(GRenderer, NULL);
	}	

	bButtonPressedLastFrame = ThePlayer->IsButton1Pressed(state);

	if (!bButtonPressedLastFrame)
	{
		bNeedToLetGoOfButton = false;
	}
}

void TextBox::Render(SDL_Renderer* Renderer)
{
	SDL_Rect DstRect = { PosX, PosY, Width, Height };
	SDL_RenderCopy(Renderer, Texture, NULL, &DstRect);

	if (bWaitingForInput && (CursorFrame % 24) > 6)
	{
		//DstRect = { PosX + Margin + CurCharIndex * FontSizeInPixels, PosY + Margin + CurrentLine * FontSizeInPixels, 32, 32 };
		DstRect = { PosX + Margin + Width - FontSizeInPixels * 2, PosY + Height - FontSizeInPixels - Margin, FontSizeInPixels, FontSizeInPixels };
		SDL_RenderCopy(Renderer, FontShadowedWhite[127].Texture, NULL, &DstRect);
	}
}

bool TextBox::IsDone()
{
	return bDone;
}

eTextBoxStyle TextBox::GetStyle()
{
	return Style;
}

TextBox::~TextBox()
{
	if (bPlotDevice)
	{
		TheGame->HandleSpecialEvent(SPECIAL_EVENT_PLOT_DEVICE);
	}
}