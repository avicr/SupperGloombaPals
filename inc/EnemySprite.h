#pragma once
#include "PhysicsSprite.h"
#include "EnemySpawnPoint.h"
#include "EmptyBlockSprite.h"

enum eTurtleState
{
	TURTLE_STATE_WALK = 0,
	TURTLE_STATE_HIDE = 1,
	TURTLE_STATE_SHELL_SLIDE = 2
};

enum eGiantGoombaState
{
	GIANT_STATE_WALK_TO_START = 0,
	GIANT_STATE_MUSHROOM,
	GIANT_STATE_CHASE
};

enum eGiantOktoState
{
	GOS_FADE_IN_EYES = 0,
	GOS_FADE_IN_BODY,
	GOS_JUMP_BACK,
	GOS_WAIT_TO_MOVE_TO_EDGE,
	GOS_MOVE_TO_EDGE,
	GOS_WAIT_TO_ATTACK,
	GOS_SHOOT_ROCK,
	GOS_RUSH_PLAYER
};

enum eOktoAttackWall
{
	OAW_SOUTH,
	OAW_NORTH,
	OAW_EAST,
	OAW_WEST
};

enum ePlantState
{
	PLANT_STATE_RETRACTED = 0,
	PLANT_STATE_GOING_UP,
	PLANT_STATE_EXTENDED,
	PLANT_STATE_GOING_DOWN,	
};

enum eSunState
{
	Sun_WaitFarEnough = 0,
	Sun_WaitThenAttackRight,
	Sun_WaitAndResetTimer20,
	Sun_WaitForUpperReturn,
	Sun_WaitThenAttackLeft,
	Sun_WaitAndResetTimer20Again,
	Sun_WaitForUpperReturn2
};

class EnemySprite : public PhysicsSprite
{
public:
	EnemySprite(EnemySpawnPoint* Spawner);
	~EnemySprite();

	virtual void HandleTileCollision(vector<TileInfo> CollisionTiles) {};
	virtual SDL_Rect GetScreenSpaceCustomRect();
	virtual bool IsDying();
	virtual bool WasBricked();
	virtual bool IsStompable();
	virtual bool IsFireable();
	virtual void GetBricked(int TileX, int TileY);
	virtual void GetStarred(int TileX, int TileY);
	virtual void GetFired();
	virtual void OnInteractedWith(EnemySprite* Other) {};
protected:
	bool bGotBricked;
	EnemySpawnPoint* MySpawner;
	int DyingCount;
};


class GoombaEnemySprite : public EnemySprite
{
	
public:	
	GoombaEnemySprite(EnemySpawnPoint* Spawner);
	virtual void Tick(double DeltaTime);
	virtual void GetStomped();	
	virtual bool Interact(EnemySprite* Enemy);		
	virtual void OnInteractedWith(EnemySprite* Other);
};

class TurtleEnemySprite : public GoombaEnemySprite
{
public:	
	virtual void GetStomped();
	TurtleEnemySprite(EnemySpawnPoint* Spawner);	
	virtual bool IsStompable();
	virtual bool IsKickable();
	virtual void GetKicked(PlayerSprite* Player);
	virtual bool Interact(EnemySprite* Enemy);
	virtual void Tick(double DeltaTime);
	virtual bool IsInteractable();
	virtual void HandleTileCollision(vector<TileInfo> CollisionTiles);
	virtual void OnInteractedWith(EnemySprite* Other);
protected:
	eTurtleState TurtleState;
	int StompCooldown;
	int KillCount;
};

class GiantGoomba : public GoombaEnemySprite
{
protected:
	eGiantGoombaState CurrentState;
	double ScaleTarget;
	double ScaleRate;
	double DestinationX;
	int ScaleCountDown;
	int JumpCount;

	void EnterState(eGiantGoombaState NewState);
	void LeaveState(eGiantGoombaState PreviousState);

	void LoadGiantTexture();	
	virtual void Interact(ItemSprite* Item);
	virtual bool Interact(EnemySprite* Enemy);

	void OnInteractedWith(EnemySprite* Other) {}
	void HandleMovement();
	virtual void Interact(Sprite *OtherSprite);

public:
	GiantGoomba(EnemySpawnPoint* Spawner);
	~GiantGoomba();

	void Tick(double DeltaTime);
	virtual SDL_Rect GetScreenSpaceCustomRect();
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum);
	bool InMapWindow(SDL_Point Offset = { 0, 0 });
	SDL_Rect GetScreenSpaceCollisionRect();
	void GetFired();
	void GetBricked(int TileX, int TileY);

	virtual void GetStomped();
	void UpdateScale();
};

class GiantOkto : public EnemySprite
{
protected:
	eOktoAttackWall TargetWall;
	eGiantOktoState CurrentState;
	int ScaleCountDown;	

	void EnterState(eGiantOktoState NewState);
	void LeaveState(eGiantOktoState PreviousState);
		
	void OnInteractedWith(EnemySprite* Other) {}
	void HandleMovement();
	void GetBricked(int TileX, int TileY);
	//virtual void Interact(Sprite *OtherSprite);

public:
	GiantOkto(int X, int Y);
	~GiantOkto();

	void Tick(double DeltaTime);	
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum);		
};

class PlantEnemySprite : public EnemySprite
{
protected:
	ePlantState CurrentState;

	// TODO: Static counter so the plants can animate in sync
	//static int AnimFrameCount;
public:
	PlantEnemySprite(EnemySpawnPoint* Spawner);

	virtual void Tick(double DeltaTime);
	void EnterState(ePlantState NewState);
	void LeaveState(ePlantState PreviousState);
	bool IsStompable() { return false; }
};

class BulletEnemySprite : public EnemySprite
{
protected:

	// The position where we should switch our render layer
	double DestinationX;
public:
	BulletEnemySprite(EnemySpawnPoint* Spawner);

	virtual void Tick(double DeltaTime);
	void GetFired() {};
	void GetBricked(int TileX, int TileY) {};
	void GetStomped();

};

class SunEnemySprite : public EnemySprite
{
protected:
	float RotationSpeed;
	float Radius;

	SDL_Point Center;
	float Angle;
	eSunState SunState;

	void EnterState(eSunState NewState);
	void LeaveState(eSunState PreviousState);

public:
	SunEnemySprite();

	virtual SDL_Rect GetScreenSpaceCollisionRect();
	virtual void Tick(double DeltaTime);
	virtual bool IsStompable();	
	virtual bool IsFireable();
	virtual void GetBricked(int TileX, int TileY);
	virtual void GetStarred(int TileX, int TileY);	
};