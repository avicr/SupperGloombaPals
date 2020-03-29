#define CHEAT
//#define FULLSCREEN_1920_1080
#define PARTY
#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#endif


//#include "../inc/Game.h"
#include <vector>
#include <string>

void DoUgly();

enum eWindow2Mode
{
	WINDOW2_MODE_NONE = 0,
	WINDOW2_MODE_WARP_ZONE,
	WINDOW2_MODE_TRANSPARENT
};

enum eDirection
{
	DIRECTION_UP = 0,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_NONE = 255
};

enum eSpecialEvent
{
	SPECIAL_EVENT_NONE = 0,
	SPECIAL_EVENT_CANCEL_CASTLE,
	SPECIAL_EVENT_PIPE_SHOOT,
	SPECIAL_EVENT_LOADING_LEVEL,
	SPECIAL_EVENT_PORTALS,
	SPECIAL_EVENT_RED_COINS_2,
	SPECIAL_EVENT_PLOT_DEVICE,
	SPECIAL_EVENT_SUN,
	SPECIAL_EVENT_LAST
};

#define TEXTURE_PATH "resource/textures/"
#define LEVEL_PATH "resource/Tilemaps/"

//#ifdef _WIN32
#define JOY_Y_AXIS 1
//#else
//#define JOY_Y_AXIS 3
//#endif
#define JOYSTICK_DEAD_ZONE 8000

// TODO: Have a separate despawn distance
#define SPAWN_DISTANCE 80 //70e

#define MIN_WALKING_VELOCITY 0.296875
#define WALKING_ACCELERATION 0.1484375
#define RUNNING_ACCELERATION 0.2265625
#define RELEASE_DECELERATION 0.17   //0.203125
#define SKID_DECELERATION 0.40625
#define MAX_RUN_SPEED 10.25
#define MAX_WALK_SPEED 6.25

#define BASE_FALL_VELOCITY 1.75
#define BASE_FALL_VELOCITY_FAST 1.50
#define BASE_FALL_VELOCITY_VERY_FAST 2.25

#define HOLDING_A_JUMP_FALL_VELOCITY 0.5
#define HOLDING_A_JUMP_FALL_VELOCITY_FAST 0.46875
#define HOLDING_A_JUMP_FALL_VELOCITY_VERY_FAST 0.625
#define VERY_FAST_VELOCITY_X 9.25
#define FAST_AIR_SPEED 6.25
#define FAST_START_JUMP_SPEED 7.25
#define AIR_FORWARD_ACCELERATION 0.1484375
#define AIR_FORWARD_ACCELERATION_FAST 0.22265625
#define AIR_BACKWARD_ACCELERATION_FAST 0.22265625
#define AIR_BACKWARD_ACCELERATION_FAST_START 0.203125
#define AIR_BACKWARD_ACCELERATION_SLOW_START 0.1484375

#define ADVENTURE_MOVE_PIXELS_PER_FRAME 5

// 10 second stars
#define NUM_STAR_FRAMES 660
#define NUM_INVINCIBLE_FRAMES 180
#define DYING_COUNT 16
#define PLAYER_DYING_COUNT 220
#define STOMP_COOL_DOWN 8
#define IN_CASTLE_FRAMES 100

#define PLAYER_SCROLL_START 312

#define TILE_TRANSPARENT_COLLISION 9

#define FIRE_ENEMY_SHOOT_FIRE_BALL_COUNT_DOWN_BASE 220
#define FIRE_ENEMY_SHOOT_FIRE_BALL_COUNT_DOWN_RANDOM 80

struct Glyph
{
	int Width;
	int Height;
	SDL_Texture *Texture;

	Glyph()
	{
		Width = 16;
		Height = 16;
		Texture = NULL;
	}

	~Glyph()
	{
		if (Texture)
		{
			SDL_DestroyTexture(Texture);
		}
	}
};

extern Glyph FontShadowedWhite[128];


using namespace std;

void DrawBitmapText(string Text, int X, int Y, int SizeX, int SizeY, SDL_Renderer *Renderer, Glyph Glyphs[127], float SpaceScaleX, float SpaceScaleY, bool bRightJustify);
Mix_Music* GetMusicFromID(int ID);
void ReadDialog(string FileName = "Dialog.txt");
void DoDialogTest(string FileName = "Dialog.txt");

extern const double GlyphSpace;
extern class ResourceManager *GResourceManager;
extern SDL_Window *GWindow;
extern SDL_Window *GWindow2;
extern SDL_Renderer * GRenderer;
extern bool bSDLInitialized;
extern double GTickDelay;
extern bool GRenderBlackout;
extern SDL_Color BGColor;
extern bool bWorldSmear;
extern int StompPoints[];
extern SDL_Point FireworkLocations[];
extern struct LevelInfo Levels[];
extern bool bShowWindow2;

extern float WindowWidth;
extern float WindowHeight;
extern float DeltaWindowYRate;
extern unsigned int SpecialEventKeys[];
extern int NormalExitKeys[];
extern int SecretExitKeys[];
extern string SpecialEventDescprtions[];

extern SDL_Joystick *Joy;
extern bool bRenderCollision;

extern class TMXMap *TheMap;
extern class PlayerSprite* ThePlayer;
extern class Game* TheGame;

template <typename T> class SpriteList;
extern SpriteList<class Sprite*> SimpleSprites;
extern SpriteList<class EnemySprite*> EnemySprites;
extern SpriteList<class ItemSprite*> ItemSprites;

extern Mix_Chunk* JumpSound;
extern Mix_Chunk* BumpSound;
extern Mix_Chunk* StompSound;
extern Mix_Chunk* BreakBrickSound;
extern Mix_Chunk* CoinSound;
extern Mix_Chunk* RedCoinSound;
extern Mix_Chunk* SurpriseSound;
extern Mix_Chunk* PowerUpSound;
extern Mix_Chunk* PowerUpGetSound;
extern Mix_Chunk* OneUpSound;
extern Mix_Chunk* FireBallSound;
extern Mix_Chunk* EventSound;
extern Mix_Chunk* KickSound;
extern Mix_Chunk* PipeSound;
extern Mix_Chunk* FlagPoleSound;
extern Mix_Chunk* TextSound;
extern Mix_Chunk* FireworkSound;
extern Mix_Chunk* AdventureHurtSound;
extern Mix_Chunk* AdventureSwordSound;

extern Mix_Music *BGMusic;
extern Mix_Music *ChaseMusic;
extern Mix_Music *UndergroundMusic;
extern Mix_Music *StarmanMusic;
extern Mix_Music *DieMusic;
extern Mix_Music *WinMusic;

extern TTF_Font * MainFont;

extern float RenderScale;

// jump and bump share a channel
#define VOLUME_NORMAL 30
#define CHANNEL_JUMP 0
#define CHANNEL_BUMP 1
#define CHANNEL_STOMP 2
#define CHANNEL_BREAK_BRICK 3
#define CHANNEL_COIN 4
#define CHANNEL_SURPRISE 5
#define CHANNEL_POWER_UP 6
#define CHANNEL_ONE_UP 7
#define CHANNEL_FIRE_BALL 8
#define CHANNEL_KICK 9
#define CHANNEL_PIPE 10
#define CHANNEL_FLAG_POLE 11
#define CHANNEL_EVENT 12
#define CHANNEL_RED_COIN 13
#define CHANNEL_FIREWORK 14
#define CHANNEL_TEXT 15

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 896

#define GIANT_GOOMBA_SIZE 320

#define OKTO_FADE_IN 80
#define OKTO_MOVE_TO_LEDGE_VELOCITY 8
#define OKTO_ATTACK_COOL_DOWN_TIME 60
#define OKTO_ATTACK_COOL_DOWN_RANDOM_MODIFIER 20
#define OKTO_JUMP_VELOCITY 26
#define OKTO_INITIAL_JUMP_BACK_COUNT 180

#define UGLY_TIME 48

void CreateWindow2();
void DestroyWindow2();
void ShowWindow2(eWindow2Mode WindowMode, bool bShow);

SDL_Renderer *GetRenderer(int RendererNumber = 0);
#endif