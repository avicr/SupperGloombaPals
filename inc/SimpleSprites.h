#pragma once
#include "Sprite.h"

class CoinEffectSprite : public Sprite
{
	int EndPosY;

public:
	CoinEffectSprite(int X, int Y);
	virtual void Tick(double DeltaTime);
};

class BrickBreakSprite : public Sprite
{
public:
	BrickBreakSprite(int X, int Y, double InVelocityX);
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
	virtual void Render(SDL_Renderer* Renderer);
};

class FlagPoleSprite : public Sprite
{
protected:
	int FlagTravelCountDown;
	// Relative to flag pol PosY
	int FlagY;	
	bool bFlagTraveling;
public:
	~FlagPoleSprite();
	FlagPoleSprite(int X, int Y);
	virtual void Tick(double DeltaTime);
	virtual void Render(SDL_Renderer* Renderer);
	virtual void Interact(class PlayerSprite* Player);
	virtual bool IsDone();
};