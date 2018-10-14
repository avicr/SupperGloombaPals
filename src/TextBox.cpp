#include "../inc/TextBox.h"
#include "../inc/PlayerSprite.h"
#include <queue>
#include <sstream>

TextBox::TextBox(int InPosX, int InPosY, int InWidth, int InHeight, string InText)
{
	SDL_assert(InText.length() < 4096);
	CharacterFrameDelay = 4;
	
	PosX = InPosX;
	PosY = InPosY;
	Width = InWidth;
	Height = InHeight;

	Text = InText;

	Texture = SDL_CreateTexture(GRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, InWidth, InHeight);
	
	SDL_SetRenderTarget(GRenderer, Texture);	
	SDL_SetRenderDrawColor(GRenderer, 0, 0, 0, 0);
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
		result.push(s);
	}

	while (result.size() != 0)
	{								
		string Word = result.front();				

		if (Word == "\l")
		{
			result.pop();
			CurrentLine++;
			CurLineLen = 0;
			Pages[CurrentPage].push_back(string());
		}
		else if (Word == "\p")
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
	FrameCount = 0;
	CurCharIndex = 0;
	bDone = false;
}

void TextBox::Tick()
{
	FrameCount++;

	if (bDone)
	{
		return;
	}	

	if (bWaitingForInput)
	{
		const Uint8 *state = SDL_GetKeyboardState(NULL);

		if (ThePlayer->IsButton1Pressed(state))
		{
			bWaitingForInput = false;

			CurrentPage++;
			CurrentLine = 0;
			FrameCount = 0;
			CurCharIndex = 0;

			SDL_SetRenderTarget(GRenderer, Texture);
			SDL_SetRenderDrawColor(GRenderer, 0, 0, 0, 0);
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
	else if (FrameCount % CharacterFrameDelay == 0)
	{
		SDL_SetRenderTarget(GRenderer, Texture);
		string CharToRender = Pages[CurrentPage][CurrentLine].substr(CurCharIndex, 1);
		DrawBitmapText(CharToRender, Margin + CurCharIndex * FontSizeInPixels, Margin + CurrentLine * FontSizeInPixels, FontSizeInPixels, FontSizeInPixels, GRenderer, FontShadowedWhite, 1, 1, false);

		CurCharIndex++;
		
		// Did we write the last character in the line?
		if (CurCharIndex >= Pages[CurrentPage][CurrentLine].length())
		{				
			// Did we write the last line in the page?
			if (CurrentLine + 1 >= Pages[CurrentPage].size())
			{
				bWaitingForInput = true;
			}
			else
			{				
				CurCharIndex = 0;
				CurrentLine++;				
			}
		}

		SDL_SetRenderTarget(GRenderer, NULL);
	}	
}

void TextBox::Render(SDL_Renderer* Renderer)
{
	SDL_Rect DstRect = { PosX, PosY, Width, Height };
	SDL_RenderCopy(Renderer, Texture, NULL, &DstRect);

	if (bWaitingForInput && (FrameCount % 24) > 6)
	{
		//DstRect = { PosX + Margin + CurCharIndex * FontSizeInPixels, PosY + Margin + CurrentLine * FontSizeInPixels, 32, 32 };
		DstRect = { PosX + Margin + Width - FontSizeInPixels * 2, PosY + Height - FontSizeInPixels - Margin, 32, 32 };
		SDL_RenderCopy(Renderer, FontShadowedWhite[127].Texture, NULL, &DstRect);
	}
}

bool TextBox::IsDone()
{
	return bDone;
}