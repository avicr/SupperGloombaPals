#pragma once
#include "Sprite.h"
#include "TMXMap.h"

enum eCoinType
{
	COIN_NORMAL,
	COIN_RED
};

enum eSpriteType
{
	ST_NONE,
	ST_OLD_MAN,
	ST_FIRE,
	ST_SWORD
};

class CoinEffectSprite : public Sprite
{
	int EndPosY;
	eCoinType CoinType;
	double FallVelocity;

public:
	CoinEffectSprite(int X, int Y, eCoinType InCoinType = COIN_NORMAL);
	virtual void Tick(double DeltaTime);
};

class BrickBreakSprite : public Sprite
{
public:
	BrickBreakSprite(int X, int Y, double InVelocityX, eTileMetaType MetaTileType = TILE_BREAKABLE);
	virtual void Tick(double DeltaTime);
};

class FireBallSprite : public Sprite
{
public:
	FireBallSprite(int X, int Y, double InVelocityX);
	~FireBallSprite();
	virtual void Tick(double DeltaTime);
	virtual bool Interact(class EnemySprite* Enemy);	
	
};

class PointSprite : public Sprite
{
protected:
	string PointString;
	int FrameCountDown;
public:
	PointSprite(int X, int Y, string Value);
	PointSprite(int X, int Y, int Value);
	virtual void Tick(double DeltaTime);
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum = 0);
};

class FlagPoleSprite : public Sprite
{
protected:
	int FlagTravelCountDown;
	// Relative to flag pol PosY
	int FlagY;	
	bool bFlagTraveling;
	int Points;
	bool bIsSecretExit;
	SDL_Texture* FlagTexture;

public:
	~FlagPoleSprite();
	FlagPoleSprite(int X, int Y, bool bSecretExit);
	virtual void Tick(double DeltaTime);
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum = 0);
	virtual void Interact(class PlayerSprite* Player);
	virtual bool IsDone();
	virtual bool IsSecretExit() { return bIsSecretExit; }
};

class PlayerFlagSprite : public Sprite
{
	int DestY;

public:
	PlayerFlagSprite(int X, int Y);
	void Tick(double DeltaTime);
};

class EventSprite : public Sprite
{
protected:
	string Caption;	
	SDL_Texture* CaptionTexture;
	SDL_Rect CaptionRect;
	SDL_Color BlendColor;
public:
	EventSprite(string InCaption);
	~EventSprite();
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum = 0);
	virtual void Tick(double DeltaTime);
};

class SpriteSpawnPoint : public Sprite
{
protected:
	eSpriteType SpriteTypeToSpawn;
	bool bUseSpawnAnimation;
	bool bInRange;
	bool bWaitingForAnimation = false;  // Flags that we are waiting for animation to stop before we spawn
	int FrameDelay; // How many frames to delay before spawning

	void SpawnSprite();

public:
	SpriteSpawnPoint(int X, int Y, eSpriteType SpriteToSpawn, bool bInUseSpawnAnimation, int InFrameDelay);
	virtual void Tick(double DeltaTime);
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum);

};

class OldManSprite : public Sprite
{
public:
	OldManSprite(int X, int Y);
	void FadeOut();

	virtual void Tick(double DeltaTime);
};

class AdventureFireSprite : public Sprite
{
public:
	AdventureFireSprite(int X, int Y);
};

class AdventureSwordAttack : public Sprite
{
public:
	AdventureSwordAttack(int X, int Y, bool bShoot);
	virtual void SetDirection(eDirection NewDirection);
	virtual SDL_Rect GetScreenSpaceCollisionRect();
	virtual void Tick(double DeltaTime);
	virtual bool Interact(class EnemySprite* Enemy);

};