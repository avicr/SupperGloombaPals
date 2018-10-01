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

enum ePlantState
{
	PLANT_STATE_RETRACTED = 0,
	PLANT_STATE_GOING_UP,
	PLANT_STATE_EXTENDED,
	PLANT_STATE_GOING_DOWN,	
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
	virtual void GetBricked(int TileX, int TileY);
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