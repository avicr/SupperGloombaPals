#include <SDL.h>
#include <SDL_mixer.h>
#include <Cmath>
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
const double GlyphSpace = 0.8;
SDL_Color BGColor = { 54, 129, 241, 255 };
SDL_Color NextTripColor = { 0, 255, 0, 255 };
SDL_Color PrevTripColor = { 0, 255, 0, 255 };
SDL_Color TripColor = { 0, 255, 0, 255 };
double BGColorLerp = 0;

SDL_Window *GWindow;
SDL_Renderer *GRenderer;
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
Mix_Chunk* SurpriseSound = NULL;
Mix_Chunk* PowerUpSound = NULL;
Mix_Chunk* PowerUpGetSound = NULL;
Mix_Chunk* OneUpSound = NULL;
Mix_Chunk* FireBallSound = NULL;
Mix_Chunk* KickSound = NULL;
Mix_Chunk* PipeSound = NULL;
Mix_Chunk* FlagPoleSound = NULL;
Mix_Chunk* FireworkSound = NULL;

Mix_Music* BGMusic = NULL;
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
	
	ThePlayer->BeginLevel();	

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
		DrawBitmapText((string)"WORLD " + TheMap->GetWorldName(), 400, 288, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.65, false);
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
			} while (DeltaTime * (double)0.001 < GTickDelay);
			//SDL_SetWindowSize(GWindow, 1024 + DeltaWindow++, 960);
			
			Tick(0.0166667);

			Render(DeltaTime * (double)0.001);			

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
		SimpleSprites.push_back(new CoinEffectSprite(ThePlayer->GetPosX(), ThePlayer->GetPosY() - 64));
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

	// Spawn fire
	if (TheEvent.key.state == SDL_PRESSED && TheEvent.key.keysym.scancode == SDL_SCANCODE_W)
	{
		ThePlayer->ShootFire();
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
	if (GRenderBlackout)
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
		ThePlayer->DrawHUD();
				
		if (bDrawDeltaTime)
		{
			DrawBitmapText(to_string(DeltaTime), 0, 0, 16, 16, GRenderer, FontShadowedWhite, 1, 1, false);
		}
			
		PresentBackBuffer();
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
			SurpriseSound = Mix_LoadWAV("resource/sounds/surprisemod.wav");
			PowerUpSound = Mix_LoadWAV("resource/sounds/powerup.wav");
			PowerUpGetSound = Mix_LoadWAV("resource/sounds/powerupget.wav");
			OneUpSound = Mix_LoadWAV("resource/sounds/oneup.wav");
			FireBallSound = Mix_LoadWAV("resource/sounds/fireball.wav");
			KickSound = Mix_LoadWAV("resource/sounds/kick.wav");
			PipeSound = Mix_LoadWAV("resource/sounds/pipe.wav");
			FlagPoleSound = Mix_LoadWAV("resource/sounds/flagpole.wav");
			FireworkSound = Mix_LoadWAV("resource/sounds/firework.wav");
			
			BGMusic = Mix_LoadMUS("resource/sounds/bgmusic.wav");
			UndergroundMusic = Mix_LoadMUS("resource/sounds/underground.wav");
			StarmanMusic = Mix_LoadMUS("resource/sounds/starman.wav");
			DieMusic = Mix_LoadMUS("resource/sounds/die.wav");
			WinMusic = Mix_LoadMUS("resource/sounds/stage_clear.wav");
			
			Mix_VolumeChunk(JumpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(BumpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(StompSound, VOLUME_NORMAL);
			Mix_VolumeChunk(BreakBrickSound, VOLUME_NORMAL);
			Mix_VolumeChunk(CoinSound, VOLUME_NORMAL);
			Mix_VolumeChunk(PowerUpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(PowerUpGetSound, VOLUME_NORMAL);
			Mix_VolumeChunk(OneUpSound, VOLUME_NORMAL);
			Mix_VolumeChunk(FireBallSound, VOLUME_NORMAL);
			Mix_VolumeChunk(KickSound, VOLUME_NORMAL);
			Mix_VolumeChunk(PipeSound, VOLUME_NORMAL);
			Mix_VolumeChunk(FlagPoleSound, VOLUME_NORMAL);
			Mix_VolumeMusic(VOLUME_NORMAL);
			
			//Mix_Volume(-1, 0);
			//Mix_VolumeMusic(0);
		}

#ifdef FULLSCREEN_1920_1080
		GWindow = SDL_CreateWindow("Supper Gloomba Pals", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
		GWindow = SDL_CreateWindow("1-1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, SCREEN_HEIGHT, SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/);
#endif
		Uint32 global_pixel_format = SDL_GetWindowPixelFormat(GWindow);
		const char* temp = SDL_GetPixelFormatName(global_pixel_format);
		printf(" SDL_Window created with pixel format %s\n", temp);

		SDL_SetWindowResizable(GWindow, SDL_TRUE);
		SDL_GetWindowSize(GWindow, &WindowWidth, &WindowHeight);
		GRenderer = SDL_CreateRenderer(GWindow, -1, 0);
		BackBuffer = SDL_CreateTexture(GRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 1024, 960);
		SDL_SetRenderTarget(GRenderer, BackBuffer);
				
		Mix_AllocateChannels(100);		
		bSDLInitialized = true;
		//SDL_ShowCursor(SDL_DISABLE);
		
		
		SDL_GetWindowPosition(GWindow, &CachedWindowX, &CachedWindowY);
		//SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
	}

	LoadBitMapFont("letters_shadow.bmp", FontShadowedWhite);	
}

SDL_Renderer *GetRenderer()
{
	if (GRenderer == NULL)
	{
		InitSDL();
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
	Mix_FreeChunk(SurpriseSound);	
	Mix_FreeChunk(OneUpSound);
	Mix_FreeChunk(KickSound);
	Mix_FreeChunk(FireBallSound);
	Mix_FreeChunk(PipeSound);
	Mix_FreeChunk(FlagPoleSound);
	Mix_FreeChunk(FireworkSound);

	Mix_FreeMusic(BGMusic);
	Mix_FreeMusic(UndergroundMusic);
	Mix_FreeMusic(StarmanMusic);
	Mix_FreeMusic(DieMusic);
	Mix_FreeMusic(WinMusic);

	SDL_DestroyRenderer(GRenderer);
	SDL_DestroyWindow(GWindow);
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
