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
	virtual void GetBricked(int TileX, int TileY);
	virtual void GetFired();
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

protected:
	eTurtleState TurtleState;
	int StompCooldown;
	int KillCount;
};