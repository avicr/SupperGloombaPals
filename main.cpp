#include <SDL.h>
#include <SDL_mixer.h>
#include <Cmath>
#include <SDL_ttf.h>
#include "SDL2_image-2.0.3\include\SDL_image.h"
#include "inc/PlayerSprite.h"
#include "inc/ResourceManager.h"
#include "inc/TMXMap.h"
#include "inc/TMXMap.h"
#include <SDL_syswm.h>
#include "inc/EnemySpawnPoint.h"
#include "inc/SimpleSprites.h"
#include "inc/SpriteList.h"
#include "inc/ItemSprite.h"
#include "inc/Game.h"

bool bDrawDeltaTime = false;
Glyph FontShadowedWhite[128];
SDL_Texture *BackBuffer;
SDL_Texture* BackBuffer2;
SDL_Texture* Texture2;
int WindowWidth;
int WindowHeight;
bool bSwapSprites = false;
bool bRenderCollision = false;
double GTickDelay = 0.01666667;
bool bWorldSmear = false;
bool bWorldRotate = false;
bool bWorldFlip = false;
float WorldAngle = 0;
float WorldAngleRate = 0.02;
bool GRenderBlackout = false;
bool bUserQuit = false;
bool bShowWindow2 = false;
const double GlyphSpace = 0.8;
SDL_Color BGColor = { 54, 129, 241, 255 };
SDL_Color NextTripColor = { 0, 255, 0, 255 };
SDL_Color PrevTripColor = { 0, 255, 0, 255 };
SDL_Color TripColor = { 0, 255, 0, 255 };
double BGColorLerp = 0;

TTF_Font* MainFont;

SDL_Window *GWindow;
SDL_Renderer *GRenderer;

SDL_Window *GWindow2 = NULL;
SDL_Renderer *GRenderer2 = NULL;

SDL_Window* UglyWindow;
SDL_Renderer* UglyRenderer;

ResourceManager *GResourceManager;
bool bSDLInitialized = false;
Uint64 TickFreq;
SDL_Joystick *Joy = NULL;
TMXMap *TheMap;
PlayerSprite* ThePlayer;
Game* TheGame;
int StompPoints[] = { 100, 200, 400, 500, 800, 1000, 2000, 4000, 5000, 8000 };
SDL_Point FireworkLocations[] = { 
	{ 426, 169 },
	{ 266, 461 },
    { 844, 265 },
	{ 842, 559 },
	{ 562, 270 },
	{ 266, 461 },
};

LevelInfo Levels[] = {
/*Level 1*/		   { LEVEL_PATH + (string)"Level1_1_64x64.tmx", "1-1", 2, 1 },	      
/*Secret Level 1*/ { LEVEL_PATH + (string)"Level2.tmx"        , "1-1S", 2, 3 },
/*Level 2*/		   { LEVEL_PATH + (string)"Level2.tmx"        , "1-2", 4, 3 },
/*Secret Level 2*/ { LEVEL_PATH + (string)"Level2S.tmx"       , "1-2S", 4, 5 },
/*Level 3*/		   { LEVEL_PATH + (string)"Level3.tmx"        , "1-3", 6, 5 },
/*Secret Level 3*/ { LEVEL_PATH + (string)"Level3S.tmx"       , "1-3S", 6, 7 },
/*Level 4*/		   { LEVEL_PATH + (string)"Level4.tmx"        , "1-4", 8, 7 },
/*Secret Level 4*/ { LEVEL_PATH + (string)"Level1_1_64x64.tmx", "2-B", 8, 9 },
/*Level 5*/	 	   { LEVEL_PATH + (string)"Level1_1_64x64.tmx", "1-5", 10, 9 },
/*Secret Level 5*/ { LEVEL_PATH + (string)"Level2.tmx"        , "1-5S", 10, 10 },
/*Level 11*/	   { LEVEL_PATH + (string)"Level1_1_64x64.tmx", "1-6", -1, -1 },
};

// Look up table to make sure save file isn't being tampared with
int SpecialEventKeys[] =
{
	0,
	640,
	6400,
	583,
	7983,
	249
};

string SpecialEventDescprtions[] =
{
	"None",
	"Ground breaking!",
	"That pipe used to work",
	"There are no loading screens",
	"Now you're thinking with portals",
	"Casual red coins"
};

int NormalExitKeys[10] =
{
	8332,
	59874,
	512,
	156,
	999,
	45812,
	59999,
	9123,
	4441,
	9781	
};

int SecretExitKeys[10] =
{
	31055,
	1242,
	8936,
	73,
	2115,
	9784,
	104,
	8883,
	2563,
	1269
};

SpriteList<Sprite*> SimpleSprites;
SpriteList<EnemySprite*> EnemySprites;
SpriteList<ItemSprite*> ItemSprites;
//SpriteList<WarpSprite*> WarpSprites;

bool bDrawUgly = false;
int DrawUglyCount = UGLY_TIME;

Mix_Chunk* JumpSound = NULL;
Mix_Chunk* BumpSound = NULL;
Mix_Chunk* StompSound = NULL;
Mix_Chunk* BreakBrickSound = NULL;
Mix_Chunk* CoinSound = NULL;
Mix_Chunk* RedCoinSound = NULL;
Mix_Chunk* SurpriseSound = NULL;
Mix_Chunk* PowerUpSound = NULL;
Mix_Chunk* PowerUpGetSound = NULL;
Mix_Chunk* OneUpSound = NULL;
Mix_Chunk* FireBallSound = NULL;
Mix_Chunk* EventSound = NULL;
Mix_Chunk* KickSound = NULL;
Mix_Chunk* PipeSound = NULL;
Mix_Chunk* FlagPoleSound = NULL;
Mix_Chunk* FireworkSound = NULL;

Mix_Music* BGMusic = NULL;
Mix_Music* ChaseMusic = NULL;
Mix_Music* UndergroundMusic = NULL;
Mix_Music* StarmanMusic = NULL;
Mix_Music* DieMusic = NULL;
Mix_Music* WinMusic = NULL;


SDL_DisplayMode Desktop;
int CachedWindowX = 0;
int CachedWindowY = 0;

float RenderScale = 1;
double RenderGrowth = 0;
double RenderGrowthRate = 0.15;

int GameLoop();
void HandleCheatInput(SDL_Event& TheEvent);
void Tick(double DeltaTime);
void Render(double DeltaTime = 0);
void PresentBackBuffer();
void PresentBackBuffer2();
void InitSDL();
int DoTitleScreen();
bool DoPreLevel();
void DoGameOver();
void CleanUp();
void UndoUgly();

void LoadBitMapFont(string FileName, Glyph *Glyphs);
void CopyGlyph(Glyph &TheGlyph, SDL_Texture *FontTexture, int TextureStartX);

float EaseIn(float t, float b, float c, float d) {
	return -c * cos(t / d * (M_PI / 2)) + c + b;
}
float EaseOut(float t, float b, float c, float d) {
	return c * sin(t / d * (M_PI / 2)) + b;
}

float EaseInOut(float t, float b, float c, float d) {
	return -c / 2 * (cos(M_PI*t / d) - 1) + b;
}


int RenderThread(void* Dat)
{
	while (1)
	{
		//Render();
		PresentBackBuffer();
		SDL_Delay(5);
	}
	return 0;
}

SDL_Texture* Ugly = NULL;

int main(int argc, char **argv)
{
	// init SDL	
	InitSDL();

	Joy = SDL_JoystickOpen(0);

	SDL_SetHint("SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "1");
	srand(SDL_GetTicks());
	SDL_SetRenderDrawColor(GRenderer, 255, 255, 255, 255);
	
	TheGame = new Game();
	TheMap = new TMXMap();	
	GResourceManager = new ResourceManager;

	ThePlayer = new PlayerSprite();
	ThePlayer->PlayAnimation(ResourceManager::PlayerGoombaAnimation);
	ThePlayer->SetWidth(64);
	ThePlayer->SetHeight(64);
	ThePlayer->SetPosition(354, 767);

	do
	{
		TheGame->StartGame();
		if (!DoTitleScreen())
		{
			break;
		}
	} while (GameLoop());

	if (Joy)
	{
		SDL_JoystickClose(Joy);
	}
	CleanUp();

	return 0;
}

int DoTitleScreen()
{
	bool bDone = false;
	
	string FileName = "TitleScreen.bmp";
	SDL_Event TheEvent;
	//SDL_Surface * Image = SDL_LoadBMP((TEXTURE_PATH + FileName).c_str());
	//SDL_Log("Loaded: %d", Image);
	//SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0, 0xFF));
	//SDL_Texture* TitleTexture = SDL_CreateTextureFromSurface(GetRenderer(), Image);
	//SDL_FreeSurface(Image);
	TheMap->ReadMap("Resource/TileMaps/TitleScreen.tmx");
	
	while (!bDone && !bUserQuit)
	{

		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE])
		{
			bDone = true;
			bUserQuit = true;
		}

		while (SDL_PollEvent(&TheEvent) != 0)
		{			
			// TODO: Add start button here
			if (TheEvent.type == SDL_JOYBUTTONDOWN)
			{
				if (TheEvent.jbutton.button == 0)
				{
					bDone = true;
				}
			}

			if (TheEvent.type == SDL_KEYDOWN)
			{				
				if (TheEvent.key.keysym.scancode == SDL_SCANCODE_RETURN || TheEvent.key.keysym.scancode == SDL_SCANCODE_SPACE)
				{
					bDone = true;
				}
			}

			if (TheEvent.type == SDL_QUIT)
			{
				bUserQuit = true;
				bDone = true;
			}
		}
				
		SDL_SetRenderDrawColor(GRenderer, BGColor.r, BGColor.g, BGColor.b, BGColor.a);
		SDL_RenderClear(GRenderer);
		TheMap->Render(GRenderer, 0, 0, 1024 / RenderScale, 960 / RenderScale);
		ThePlayer->DrawHUD();
		PresentBackBuffer();
	}	
	
	TheGame->EndLevel();
	return !bUserQuit;
}

bool DoPreLevel()
{
	bool bDone = false;
	Uint64 StartTime = SDL_GetPerformanceCounter();
	Uint64 CurrentTime = SDL_GetPerformanceCounter();
	double DeltaTime;
	double SecondsCountdown = 2.5;
	bool bLevelLoaded = false;

	SDL_Event TheEvent;
	
	//ThePlayer->BeginLevel();	

	while (!bDone && !bUserQuit)
	{	
		if (!bLevelLoaded)
		{
			TheGame->LoadCurrentLevel();
		}

		bLevelLoaded = true;

		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE])
		{
			bDone = true;
			bUserQuit = true;
		}
		StartTime = CurrentTime;
				

		StartTime = CurrentTime;

		// Only tick at 60 fps
		do
		{
			CurrentTime = SDL_GetPerformanceCounter();
			DeltaTime = (double)((CurrentTime - StartTime) * 1000 / (double)SDL_GetPerformanceFrequency());
		} while (DeltaTime * (double)0.001 < GTickDelay);

		SecondsCountdown -= DeltaTime * (double)0.001;

		if (SecondsCountdown <= 0)
		{
			bDone = true;
		}
		SDL_SetRenderDrawColor(GRenderer, 0, 0, 0, 255);
		SDL_RenderClear(GRenderer);
		DrawBitmapText((string)"WORLD " + TheGame->GetWorldName(), 400, 288, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.65, false);
		SDL_Rect SrcRect = { 0, 0, 64, 64 };
		SDL_Rect DstRect = { 432, 370, 48, 48 };
		/*ThePlayer->SetPosition(432, 396);
		ThePlayer->SetWidth(48);
		ThePlayer->SetHeight(48);*/
		//ThePlayer->Render(GRenderer);		

		SDL_RenderCopy(GRenderer, GResourceManager->PlayerGoombaTexture->Texture, &SrcRect, &DstRect);
		
		char Temp[10];
		itoa(ThePlayer->GetLives(), Temp, 10);
		string LivesString = (string)"X  " + Temp;
		DrawBitmapText(LivesString, 500, 388, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.65, false);

		ThePlayer->DrawHUD();

		PresentBackBuffer();

		while (SDL_PollEvent(&TheEvent) != 0)
		{			
			if (TheEvent.type == SDL_QUIT)
			{
				bUserQuit = true;
				bDone = true;
			}
		}

	}	
	
	return !bUserQuit;
}

void DoGameOver()
{
	bool bDone = false;
	Uint64 StartTime = SDL_GetPerformanceCounter();
	Uint64 CurrentTime = SDL_GetPerformanceCounter();
	double DeltaTime;
	double SecondsCountdown = 3.5;	

	SDL_Event TheEvent;
	
	while (!bDone && !bUserQuit)
	{

		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE])
		{
			bDone = true;
			bUserQuit = true;
		}
		StartTime = CurrentTime;
		
		do
		{
			CurrentTime = SDL_GetPerformanceCounter();
			DeltaTime = (double)((CurrentTime - StartTime) * 1000 / (double)SDL_GetPerformanceFrequency());
		} while (DeltaTime * (double)0.001 < GTickDelay);

		SecondsCountdown -= DeltaTime * (double)0.001;

		if (SecondsCountdown <= 0)
		{
			bDone = true;
		}

		SDL_SetRenderDrawColor(GRenderer, 0, 0, 0, 255);
		SDL_RenderClear(GRenderer);
		DrawBitmapText("GAME OVER", 400, 288, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.65, false);				
		PresentBackBuffer();

		while (SDL_PollEvent(&TheEvent) != 0)
		{
			if (TheEvent.type == SDL_QUIT)
			{
				bUserQuit = true;
				bDone = true;
			}
		}

	}
}

int GameLoop()
{
	int DeltaWindow = 0;
	bool bDone = false;
	const int ScrollRate = 5;
	bool bGameDone = false;
	SDL_Event TheEvent;
	TickFreq = SDL_GetPerformanceFrequency();
	//SDL_SetWindowBordered(GWindow, SDL_FALSE);
	Uint64 StartTime = SDL_GetPerformanceCounter();
	Uint64 CurrentTime = SDL_GetPerformanceCounter();
	double DeltaTime;

	const Uint8* state = SDL_GetKeyboardState(NULL);
	SDL_SetRenderDrawColor(GRenderer, BGColor.r, BGColor.g, BGColor.b, BGColor.a);
	SDL_RenderClear(GRenderer);
	//SDL_CreateThread(RenderThread, "render", NULL);

	while (ThePlayer->GetLives() > 0 && DoPreLevel())
	{
		RenderGrowth = 0;
		RenderGrowthRate = 0.15;
		TheGame->StartLevel();
		while (!bDone && !ThePlayer->IsDead() && !TheGame->IsLevelComplete())
		{
			if (state[SDL_SCANCODE_ESCAPE])
			{
				bDone = true;
				bUserQuit = true;
			}

			StartTime = CurrentTime;

			// Only tick at 60 fps
			do
			{
				CurrentTime = SDL_GetPerformanceCounter();
				DeltaTime = (double)((CurrentTime - StartTime) * 1000 / (double)SDL_GetPerformanceFrequency());
				DeltaTime *= (double)0.001;
			} while (DeltaTime < GTickDelay && !TheGame->IsLevelComplete());
			//SDL_SetWindowSize(GWindow, 1024 + DeltaWindow++, 960);
			
			Tick(0.0166667);

			Render(DeltaTime);

			//Handle events on queue
			while (SDL_PollEvent(&TheEvent) != 0)
			{
				if (TheEvent.type == SDL_WINDOWEVENT)
				{
					if (TheEvent.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						/*ThePlayer->SetPosition(ThePlayer->GetPosX() - (TheEvent.window.data1 - CachedWindowX), ThePlayer->GetPosY() - (TheEvent.window.data2 - CachedWindowY));
						*/
						CachedWindowX = TheEvent.window.data1;
						CachedWindowY = TheEvent.window.data2;

					}
				}

#ifdef CHEAT

				HandleCheatInput(TheEvent);
#endif
				//User requests quit
				if (TheEvent.type == SDL_QUIT)
				{
					bDone = true;
					bUserQuit = true;
				}
			}			
		}
		TheGame->EndLevel();		
	}

	if (ThePlayer->GetLives() <= 0)
	{
		 DoGameOver();		
	}
	
	TheGame->EndGame();
	delete ThePlayer;
	ThePlayer = new PlayerSprite();
	return !bUserQuit;
}

void HandleCheatInput(SDL_Event& TheEvent)
{
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_1)
	{
		bRenderCollision = !bRenderCollision;
	}

	// Draw deltatime
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_T)
	{
		bDrawDeltaTime = !bDrawDeltaTime;
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_MINUS)
	{
		GTickDelay /= 0.75;
		SDL_Log("Tick rate: %f", GTickDelay);
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_EQUALS)
	{
		GTickDelay *= 0.75;
		SDL_Log("Tick rate: %f", GTickDelay);
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_3)
	{
		ThePlayer->SetPowerUpState(POWER_BIG);
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_4)
	{
		SimpleSprites.push_back(new CoinEffectSprite(ThePlayer->GetPosX(), ThePlayer->GetPosY() - 64, COIN_RED));
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_5)
	{
		if (!bDrawUgly)
		{
			DoUgly();
		}
		else
		{
			UndoUgly();
		}
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_6)
	{
		ItemSprite* NewEnemy = new BigMushroomItemSprite(ThePlayer->GetPosX(), ThePlayer->GetPosY());
		NewEnemy->SetWidth(64);
		NewEnemy->SetHeight(64);
		//EnemySprites.push_back(NewEnemy);
		ItemSprites.push_back(NewEnemy);
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_7)
	{
		ItemSprite* NewEnemy = new OneUpMushroomSprite(ThePlayer->GetPosX(), ThePlayer->GetPosY());
		NewEnemy->SetWidth(64);
		NewEnemy->SetHeight(64);
		//EnemySprites.push_back(NewEnemy);
		ItemSprites.push_back(NewEnemy);
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_8)
	{
		int SpawnX = ThePlayer->GetPosX() / 64 * 64;
		int SpawnY = ThePlayer->GetPosY() / 64 * 64;
		SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY, -4));
		SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY, 4));
		SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY + 32, -4));
		SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY + 32, 4));
	}

	// Remove background
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_9)
	{
		bWorldSmear = !bWorldSmear;
	}

	// Spawn flower
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_Q)
	{
		int SpawnX = ThePlayer->GetPosX() / 64 * 64 + 64;
		int SpawnY = ThePlayer->GetPosY() / 64 * 64;
		FlowerItemSprite* FlowerSprite = new FlowerItemSprite(SpawnX, SpawnY);
		ItemSprites.push_back(FlowerSprite);
	}

	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_E)
	{
		SimpleSprites.push_back(new EventSprite("This is an event!"));
	}

	// Spawn star
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_W)
	{
		int SpawnX = ThePlayer->GetPosX() / 64 * 64 + 64;
		int SpawnY = ThePlayer->GetPosY() / 64 * 64;
		StarItemSprite* FlowerSprite = new StarItemSprite(SpawnX, SpawnY);
		ItemSprites.push_back(FlowerSprite);
	}

	// Spawn empty brick sprite
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_0)
	{
		Sprite* NewBlock = new Sprite(GResourceManager->EmptyBlockSpriteTexture->Texture);
		NewBlock->SetPosition(ThePlayer->GetPosX(), ThePlayer->GetPosY() - 128);
		NewBlock->SetWidth(64);
		NewBlock->SetHeight(128);

		SimpleSprites.push_back(NewBlock);
		NewBlock->PlayAnimation(GResourceManager->EmptyBlockSpriteAnimation, false);
	}

	// Spawn goomba spawn point
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_2)
	{
		//EnemySpawnPoint* NewBlock = new EnemySpawnPoint(ENEMY_TURTLE, (int)ThePlayer->GetPosX() / (int)64  * (int)64 + SPAWN_DISTANCE + SCREEN_WIDTH, (int)ThePlayer->GetPosY() / (int)64 * (int)64 + 64);
		EnemySpawnPoint* NewBlock = new EnemySpawnPoint(ENEMY_TURTLE, (int)ThePlayer->GetPosX() / (int)64 * (int)64 + 512, (int)ThePlayer->GetPosY() / (int)64 * (int)64 + 64);
		SimpleSprites.push_back(NewBlock);
		NewBlock->SpawnEnemy();
	}

	// Warps
	for (int i = 0; i < 8; i++)
	{
		if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_F1 + i)
		{
			int WarpIndex = TheMap->GetWarpIndex(i);
			vector<WarpInfo> Warps = TheMap->GetWarps();

			if (WarpIndex > -1)
			{
				if (Warps[WarpIndex].Entrances.size() == 0)
				{
					Warps[WarpIndex].Entrances.push_back(WarpEntrance());
				}
				ThePlayer->StartWarpSequence(Warps[WarpIndex].Entrances[0], Warps[WarpIndex].Exits[0]);
			}
		}
	}
}

void Tick(double DeltaTime)
{
	if (bDrawUgly)
	{
		DrawUglyCount -= 1;

		if (DrawUglyCount <= 0)
		{
			UndoUgly();
		}
	}
	else
	{
		TheMap->Tick(DeltaTime);
		SimpleSprites.Tick(DeltaTime);

		if (!ThePlayer->IsChangingSize())
		{
			EnemySprites.Tick(DeltaTime);
			ItemSprites.Tick(DeltaTime);
		}
		ThePlayer->Tick(DeltaTime);
		TheGame->Tick();
	}
}

void Render(double DeltaTime)
{	
	// Clear to ground color
	if (bDrawUgly)
	{		
	}
	else if (GRenderBlackout)
	{
		SDL_SetRenderDrawColor(GRenderer, 0, 0, 0, 255);
		SDL_RenderClear(GRenderer);
		PresentBackBuffer();
	}
	else
	{		
		if (!bWorldSmear)
		{
			SDL_SetRenderDrawColor(GRenderer, BGColor.r, BGColor.g, BGColor.b, BGColor.a);
			SDL_RenderClear(GRenderer);
		}						
		TheMap->Render(GRenderer, 0, 0, 1024 / RenderScale, 960 / RenderScale);		
				
		if (bDrawDeltaTime)
		{
			DrawBitmapText(to_string(DeltaTime), 0, 0, 16, 16, GRenderer, FontShadowedWhite, 1, 1, false);
		}
	
		ThePlayer->DrawHUD();

		SimpleSprites.Render(GRenderer, RENDER_LAYER_ABOVE_ALL);
		PresentBackBuffer();

		if (bShowWindow2)
		{
			SDL_SetRenderDrawColor(GRenderer2, BGColor.r, BGColor.g, BGColor.b, BGColor.a);
			SDL_RenderClear(GRenderer2);

			double CachedX = ThePlayer->GetPosX();
			double CachedY = ThePlayer->GetPosY();

			ThePlayer->SetPosition(CachedX - 195 * 64 + TheMap->GetScrollX(), CachedY - 30 * 64 + TheMap->GetScrollY());
			ThePlayer->Render(GRenderer2, 2);
			ThePlayer->SetPosition(CachedX, CachedY);

			SDL_RenderCopy(GRenderer2, Texture2, NULL, NULL);
			SDL_Rect SrcRect = { 0, 0, ThePlayer->GetWidth(), ThePlayer->GetHeight() };
			SDL_Rect DstRect = { ThePlayer->GetPosX() - TheMap->GetScrollX(), ThePlayer->GetPosY() - TheMap->GetScrollY() - 448, SrcRect.w, SrcRect.h };


			PresentBackBuffer2();
		}
	}	
}

void DoTripStuff()
{
	if (ThePlayer->GetTripLevel() >= 1 && ThePlayer->GetTripLevel() < 4)
	{
		RenderGrowth += RenderGrowthRate;

		if (RenderGrowth >= 15)
		{
			RenderGrowthRate *= -1;
		}

		if (RenderGrowth <= 0)
		{
			RenderGrowthRate *= -1;
		}
	} 
	else
	{
		RenderGrowth = 0;
	}

	/*if (ThePlayer->GetTripLevel() >= 2 && ThePlayer->GetTripLevel() < 4)
	{
		if ((TripColor.r == NextTripColor.r && TripColor.g == NextTripColor.g && TripColor.b == NextTripColor.b) || BGColorLerp > 2)
		{
			NextTripColor.r = rand() % 128 + 128;
			NextTripColor.g = rand() % 128 + 128;
			NextTripColor.b = rand() % 128 + 128;
			BGColorLerp = 0;
			PrevTripColor = BGColor;
		}
		else
		{
			TripColor.r = EaseOut(BGColorLerp, PrevTripColor.r, NextTripColor.r, 1);
			TripColor.g = EaseOut(BGColorLerp, PrevTripColor.g, NextTripColor.g, 1);
			TripColor.b = EaseOut(BGColorLerp, PrevTripColor.b, NextTripColor.b, 1);

			BGColorLerp += GTickDelay;
		}

		BGColor = TripColor;
	}*/
}

void PresentBackBuffer()
{
	SDL_SetRenderTarget(GRenderer, NULL);
	SDL_Rect BackBufferRect = { 0, 0, 1024, SCREEN_HEIGHT };
#ifdef FULLSCREEN_1920_1080
	SDL_Rect BackBufferDstRect = { 42, 0, 1836, 1080 };
#else
	//SDL_Rect BackBufferDstRect = { WindowWidth / 2 - 512, WindowHeight / 2 - SCREEN_HEIGHT / 2, 1024, SCREEN_HEIGHT };
	SDL_Rect BackBufferDstRect = { -RenderGrowth, -RenderGrowth, 1024 + RenderGrowth * 4, SCREEN_HEIGHT + RenderGrowth * 4};
#endif

	DoTripStuff();
	
	//SDL_RenderCopy(GRenderer, BackBuffer, &BackBufferRect, &BackBufferDstRect);
	SDL_RenderCopyEx(GRenderer, BackBuffer, &BackBufferRect, &BackBufferDstRect, /*Angle = Angle + 0.02*/0, NULL, SDL_FLIP_NONE);
	SDL_RenderPresent(GRenderer);
	SDL_SetRenderTarget(GRenderer, BackBuffer);
}

void PresentBackBuffer2()
{
	SDL_SetRenderTarget(GRenderer2, NULL);
	SDL_Rect BackBufferRect = { 0, 0, 512, 512};
#ifdef FULLSCREEN_1920_1080
	SDL_Rect BackBufferDstRect = { 42, 0, 1836, 1080 };
#else
	//SDL_Rect BackBufferDstRect = { WindowWidth / 2 - 512, WindowHeight / 2 - SCREEN_HEIGHT / 2, 1024, SCREEN_HEIGHT };
	SDL_Rect BackBufferDstRect = { 0, 0, 512, 512};
#endif

	DoTripStuff();

	//SDL_RenderCopy(GRenderer, BackBuffer, &BackBufferRect, &BackBufferDstRect);
	SDL_RenderCopyEx(GRenderer2, BackBuffer2, &BackBufferRect, &BackBufferDstRect, /*Angle = Angle + 0.02*/0, NULL, SDL_FLIP_NONE);
	SDL_RenderPresent(GRenderer2);
	SDL_SetRenderTarget(GRenderer2, BackBuffer2);
}

void InitSDL()
{
	if (!bSDLInitialized)
	{

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
		
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
		{			
		}
		else
		{			

			JumpSound = Mix_LoadWAV("resource/sounds/jump.wav");
			BumpSound = Mix_LoadWAV("resource/sounds/bump.wav");
			StompSound = Mix_LoadWAV("resource/sounds/stomp.wav");
			BreakBrickSound = Mix_LoadWAV("resource/sounds/breakbrick.wav");
			CoinSound = Mix_LoadWAV("resource/sounds/coin.wav");
			RedCoinSound = Mix_LoadWAV("resource/sounds/redcoin.wav");
			SurpriseSound = Mix_LoadWAV("resource/sounds/surprisemod.wav");
			PowerUpSound = Mix_LoadWAV("resource/sounds/powerup.wav");
			PowerUpGetSound = Mix_LoadWAV("resource/sounds/powerupget.wav");
			OneUpSound = Mix_LoadWAV("resource/sounds/oneup.wav");
			FireBallSound = Mix_LoadWAV("resource/sounds/fireball.wav");
			EventSound = Mix_LoadWAV("resource/sounds/event.wav");
			KickSound = Mix_LoadWAV("resource/sounds/kick.wav");
			PipeSound = Mix_LoadWAV("resource/sounds/pipe.wav");
			FlagPoleSound = Mix_LoadWAV("resource/sounds/flagpole.wav");
			FireworkSound = Mix_LoadWAV("resource/sounds/firework.wav");
			
			BGMusic = Mix_LoadMUS("resource/sounds/bgmusic.wav");
			ChaseMusic = Mix_LoadMUS("resource/sounds/chase.wav");
			UndergroundMusic = Mix_LoadMUS("resource/sounds/underground.wav");
			StarmanMusic = Mix_LoadMUS("resource/sounds/starman.wav");
			DieMusic = Mix_LoadMUS("resource/sounds/die.wav");
			WinMusic = Mix_LoadMUS("resource/sounds/stage_clear.wav");
			
			Mix_VolumeChunk(JumpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(BumpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(StompSound, VOLUME_NORMAL);
			Mix_VolumeChunk(BreakBrickSound, VOLUME_NORMAL);
			Mix_VolumeChunk(CoinSound, VOLUME_NORMAL);
			Mix_VolumeChunk(RedCoinSound, VOLUME_NORMAL);
			Mix_VolumeChunk(PowerUpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(PowerUpGetSound, VOLUME_NORMAL);
			Mix_VolumeChunk(OneUpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(FireBallSound, VOLUME_NORMAL);
			Mix_VolumeChunk(EventSound, VOLUME_NORMAL + 2);
			Mix_VolumeChunk(KickSound, VOLUME_NORMAL);
			Mix_VolumeChunk(PipeSound, VOLUME_NORMAL);
			Mix_VolumeChunk(FlagPoleSound, VOLUME_NORMAL);
			Mix_VolumeChunk(FireworkSound, VOLUME_NORMAL);
			Mix_VolumeMusic(VOLUME_NORMAL);
			
			/*Mix_Volume(-1, 0);
			Mix_VolumeMusic(0);*/
		}

#ifdef FULLSCREEN_1920_1080
		GWindow = SDL_CreateWindow("Supper Gloomba Pals", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
		GWindow = SDL_CreateWindow("1-1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, SCREEN_HEIGHT, SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/);
		
#endif		
		CreateWindow2();

		Uint32 global_pixel_format = SDL_GetWindowPixelFormat(GWindow);
		const char* temp = SDL_GetPixelFormatName(global_pixel_format);
		printf(" SDL_Window created with pixel format %s\n", temp);

		//SDL_SetWindowResizable(GWindow, SDL_TRUE);
		SDL_GetWindowSize(GWindow, &WindowWidth, &WindowHeight);
		GRenderer = SDL_CreateRenderer(GWindow, -1, 0);
		BackBuffer = SDL_CreateTexture(GRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 1024, 960);
		
		SDL_SetRenderTarget(GRenderer, BackBuffer);
		
				
		Mix_AllocateChannels(100);		
		bSDLInitialized = true;
		//SDL_ShowCursor(SDL_DISABLE);
				
		//SDL_GetWindowPosition(GWindow, &CachedWindowX, &CachedWindowY);		

		//SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

		TTF_Init();
		MainFont = TTF_OpenFont("resource/Heebo-Medium.ttf", 20);

		LoadBitMapFont("letters_shadow.bmp", FontShadowedWhite);
	}
	
}

SDL_Renderer *GetRenderer(int RendererNumber)
{
	if (GRenderer == NULL)
	{
		InitSDL();
	}

	if (RendererNumber == 2)
	{
		return GRenderer2;
	}	

	return GRenderer;
}

void CleanUp()
{
	delete GResourceManager;
	
	//TTF_Quit();

	if (Ugly)
	{
		SDL_DestroyTexture(Ugly);
	}

	Mix_FreeChunk(JumpSound);
	Mix_FreeChunk(BumpSound);
	Mix_FreeChunk(StompSound);
	Mix_FreeChunk(BreakBrickSound);
	Mix_FreeChunk(CoinSound);
	Mix_FreeChunk(RedCoinSound);
	Mix_FreeChunk(SurpriseSound);	
	Mix_FreeChunk(OneUpSound);
	Mix_FreeChunk(KickSound);
	Mix_FreeChunk(FireBallSound);
	Mix_FreeChunk(EventSound);
	Mix_FreeChunk(PipeSound);
	Mix_FreeChunk(FlagPoleSound);
	Mix_FreeChunk(FireworkSound);

	Mix_FreeMusic(BGMusic);
	Mix_FreeMusic(ChaseMusic);
	Mix_FreeMusic(UndergroundMusic);
	Mix_FreeMusic(StarmanMusic);
	Mix_FreeMusic(DieMusic);
	Mix_FreeMusic(WinMusic);

	SDL_DestroyRenderer(GRenderer);
	SDL_DestroyWindow(GWindow);
	DestroyWindow2();
	SDL_Quit();
}

void DoUgly()
{
	Mix_Volume(-1, 0);
	Mix_VolumeMusic(0);

	SDL_GetDesktopDisplayMode(0, &Desktop);
	SDL_SetWindowPosition(GWindow, 0, 0);
	SDL_SetWindowBordered(GWindow, SDL_FALSE);
	SDL_SetWindowSize(GWindow, Desktop.w, Desktop.h);
	string FileName = "large.bmp";

	SDL_Surface * Image = SDL_LoadBMP((TEXTURE_PATH + FileName).c_str());
	SDL_Log("Loaded: %d", Image);
	
	Ugly = SDL_CreateTextureFromSurface(GetRenderer(), Image);	
	SDL_FreeSurface(Image);

	SDL_DestroyTexture(BackBuffer);
	BackBuffer = SDL_CreateTexture(GRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, Desktop.w, Desktop.h);
	SDL_SetRenderTarget(GRenderer, BackBuffer);
	SDL_Rect SrcRect = { 0, 0, Desktop.w, Desktop.h};
	SDL_Rect DstRect = { 0, 0, Desktop.w, Desktop.h };
	SDL_RenderCopy(GRenderer, Ugly, &SrcRect, &DstRect);
	
	SDL_SetRenderTarget(GRenderer, NULL);
	SDL_Rect BackBufferRect = { 0, 0, Desktop.w, Desktop.h};

	SDL_RenderCopy(GRenderer, BackBuffer, &BackBufferRect, &BackBufferRect);
	SDL_RenderPresent(GRenderer);
	SDL_SetRenderTarget(GRenderer, BackBuffer);

	Mix_VolumeChunk(SurpriseSound, 90);
	Mix_Volume(CHANNEL_SURPRISE, 128);
	Mix_PlayChannel(CHANNEL_SURPRISE, SurpriseSound, 0);
	TTF_Quit();
	bDrawUgly = true;
}

void UndoUgly()
{
	Mix_HaltChannel(CHANNEL_SURPRISE);
	
	Mix_Volume(-1, 128);
	Mix_VolumeMusic(VOLUME_NORMAL);
		
	SDL_SetWindowBordered(GWindow, SDL_TRUE);
#ifdef FULLSCREEN_1920_1080
	SDL_SetWindowSize(GWindow, 1920, 1080);
#else
	SDL_SetWindowSize(GWindow, 1024, SCREEN_HEIGHT);
#endif
	SDL_SetWindowPosition(GWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	SDL_DestroyTexture(Ugly);
	Ugly = NULL;

	SDL_DestroyTexture(BackBuffer);

	BackBuffer = SDL_CreateTexture(GRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 1024, 960);
	SDL_SetRenderTarget(GRenderer, BackBuffer);	

	DrawUglyCount = UGLY_TIME;
	bDrawUgly = false;
	LoadBitMapFont("letters_shadow.bmp", FontShadowedWhite);
}

void LoadBitMapFont(string FileName, Glyph *Glyphs)
{
	SDL_SetRenderTarget(GRenderer, NULL);

	SDL_Surface *Image = SDL_LoadBMP((TEXTURE_PATH + FileName).c_str());
	SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0, 0xFF));
	SDL_Texture *FontTexture = SDL_CreateTextureFromSurface(GRenderer, Image);

	// Numbers start at X 16 in the texture
	for (int i = 48; i < 58; i++)
	{
		CopyGlyph(Glyphs[i], FontTexture, 16 + (i - 48) * 16);
	}

	// A starts at X 176
	for (int i = 65; i < 91; i++)
	{
		CopyGlyph(Glyphs[i], FontTexture, 176 + (i - 65) * 16);
	}

	CopyGlyph(Glyphs[' '], FontTexture, 0);
	CopyGlyph(Glyphs['.'], FontTexture, 592);
	CopyGlyph(Glyphs['-'], FontTexture, 608);
	CopyGlyph(Glyphs['!'], FontTexture, 624);
	CopyGlyph(Glyphs['?'], FontTexture, 640);

	SDL_FreeSurface(Image);
	SDL_DestroyTexture(FontTexture);
}

void CopyGlyph(Glyph &TheGlyph, SDL_Texture *FontTexture, int TextureStartX)
{
	SDL_Rect SrcRect = { TextureStartX, 0, 16, 16 };

	if (TheGlyph.Texture)
	{
		SDL_DestroyTexture(TheGlyph.Texture);
	}
	TheGlyph.Texture = SDL_CreateTexture(GRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 16, 16);

	SDL_SetRenderTarget(GRenderer, TheGlyph.Texture);
	SDL_SetTextureBlendMode(TheGlyph.Texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(GRenderer, 255, 0, 255, 0);
	SDL_RenderClear(GRenderer);
	SDL_Rect DstRect = { 0, 0, 16, 16 };
	SDL_RenderCopy(GRenderer, FontTexture, &SrcRect, &DstRect);
	SDL_SetRenderTarget(GRenderer, NULL);
}

void DrawBitmapText(string Text, int X, int Y, int SizeX, int SizeY, SDL_Renderer *Renderer, Glyph Glyphs[127], float SpaceScaleX, float SpaceScaleY, bool bRightJustify)
{
	double PosX = X;
	double PosY = Y;

	if (!bRightJustify)
	{
		for (int i = 0; i < Text.size(); i++)
		{

			char CharToRender = Text.at(i);

			if (!Glyphs[CharToRender].Texture && CharToRender != '\n')
			{
				continue;
			}

			if (CharToRender == '\n')
			{

				PosY += SizeY * SpaceScaleY;
				PosX = X;
			}
			else if (CharToRender >= 0 && CharToRender < 94)
			{
				SDL_Rect SrcRect = { 0, 0, Glyphs[CharToRender].Width, Glyphs[CharToRender].Height };
				SDL_Rect DstRect = { PosX, PosY, SizeX, SizeY };

				SDL_RenderCopy(Renderer, Glyphs[CharToRender].Texture, &SrcRect, &DstRect);

				PosX += SizeX * SpaceScaleX;
			}
		}
	}
	else
	{
		PosX -= SizeX;
		for (int i = Text.size() - 1; i >= 0; i--)
		{
			char CharToRender = Text.at(i);

			if (!Glyphs[CharToRender].Texture && CharToRender != '\n')
			{
				continue;
			}

			SDL_Rect SrcRect = { 0, 0, Glyphs[CharToRender].Width, Glyphs[CharToRender].Height };
			SDL_Rect DstRect = { PosX, Y, SizeX, SizeY };

			SDL_RenderCopy(Renderer, Glyphs[CharToRender].Texture, &SrcRect, &DstRect);

			PosX -= SizeX * SpaceScaleX;
		}
	}
}

Mix_Music* GetMusicFromID(int ID)
{
	Mix_Music* Music = NULL;
	switch (ID)
	{
	case 0:
		Music = NULL;
		break;
	case 1:
		Music = BGMusic;
		break;
	case 2:
		Music = UndergroundMusic;
		break;

	default:
		break;
	}
	return Music;
}

void SetUpEvents()
{
	Levels[0].SpecialEvents.push_back(SPECIAL_EVENT_CANCEL_CASTLE);
}

void CreateWindow2()
{
	int Window1X;
	int Window1Y;
	int Window2X;
	int Window2Y;
	int Window1Width;
	int Window1Height;
	int Window2Width;
	int Window2Height;	

	GWindow2 = SDL_CreateWindow("Warp Zone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	GRenderer2 = SDL_CreateRenderer(GWindow2, -1, 0);
	BackBuffer2 = SDL_CreateTexture(GRenderer2, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 512, 512);

	SDL_GetWindowPosition(GWindow, &Window1X, &Window1Y);
	
	SDL_GetWindowSize(GWindow, &Window1Width, &Window1Height);
	SDL_SetWindowPosition(GWindow2, Window1X + WindowWidth, Window1Y);

	string FileName = "otherwindow.bmp";
	SDL_Surface* Image = SDL_LoadBMP((TEXTURE_PATH + FileName).c_str());

	SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0, 0xFF));
	Texture2 = SDL_CreateTextureFromSurface(GRenderer2, Image);
	SDL_FreeSurface(Image);
	SDL_SetRenderTarget(GRenderer2, BackBuffer2);
}

void DestroyWindow2()
{
	SDL_DestroyRenderer(GRenderer2);
	SDL_DestroyWindow(GWindow2);
	SDL_DestroyTexture(Texture2);

	GWindow2 = NULL;
	GRenderer2 = NULL;
	Texture2 = NULL;
}

void ShowWindow2(bool bShow)
{
	if (bShow)
	{
		int Window1X;
		int Window1Y;
		int Window2X;
		int Window2Y;
		int Window1Width;
		int Window1Height;
		int Window2Width;
		int Window2Height;

		SDL_GetWindowPosition(GWindow, &Window1X, &Window1Y);

		SDL_GetWindowSize(GWindow, &Window1Width, &Window1Height);
		SDL_SetWindowPosition(GWindow2, Window1X + WindowWidth, Window1Y);
		SDL_ShowWindow(GWindow2);
	}
	else
	{
		SDL_HideWindow(GWindow2);
	}

	bShowWindow2 = bShow;
}