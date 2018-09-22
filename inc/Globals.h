#define CHEAT
//#define FULLSCREEN_1920_1080
#define PARTY
#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef _WIN32
#include <SDL.h>
#include <SDL_mixer.h>
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#endif


//#include "../inc/Game.h"
#include <vector>
#include <string>

void DoUgly();

enum eDirection
{
	DIRECTION_UP = 0,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_NONE = 255
};

#define TEXTURE_PATH "resource/textures/"

//#ifdef _WIN32
#define JOY_Y_AXIS 1
//#else
//#define JOY_Y_AXIS 3
//#endif
#define JOYSTICK_DEAD_ZONE 8000

// TODO: Have a separate despawn distance
#define SPAWN_DISTANCE 70

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

// 10 second stars
#define NUM_STAR_FRAMES 660
#define NUM_INVINCIBLE_FRAMES 180
#define DYING_COUNT 16
#define PLAYER_DYING_COUNT 220
#define STOMP_COOL_DOWN 8

#define PLAYER_SCROLL_START 312

#define TILE_TRANSPARENT_COLLISION 9

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

extern const double GlyphSpace;
extern class ResourceManager *GResourceManager;
extern SDL_Window *GWindow;
extern SDL_Renderer * GRenderer;
extern bool bSDLInitialized;
extern double GTickDelay;
extern bool GRenderBlackout;
extern SDL_Color BGColor;
extern bool bWorldSmear;
extern int StompPoints[];

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
extern Mix_Chunk* SurpriseSound;
extern Mix_Chunk* PowerUpSound;
extern Mix_Chunk* PowerUpGetSound;
extern Mix_Chunk* OneUpSound;
extern Mix_Chunk* FireBallSound;
extern Mix_Chunk* KickSound;
extern Mix_Chunk* PipeSound;
extern Mix_Music *BGMusic;
extern Mix_Music *UndergroundMusic;
extern Mix_Music *StarmanMusic;
extern Mix_Music *DieMusic;

extern float RenderScale;

// jump and bump share a channel
#define VOLUME_NORMAL 30
#define CHANNEL_JUMP 0
#define CHANNEL_BUMP 0
#define CHANNEL_STOMP 2
#define CHANNEL_BREAK_BRICK 3
#define CHANNEL_COIN 4
#define CHANNEL_SURPRISE 5
#define CHANNEL_POWER_UP 6
#define CHANNEL_ONE_UP 7
#define CHANNEL_FIRE_BALL 8
#define CHANNEL_KICK 9
#define CHANNEL_PIPE 10

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 896

#define UGLY_TIME 48

SDL_Renderer *GetRenderer();
#endif