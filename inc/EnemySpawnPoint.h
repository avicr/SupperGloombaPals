#pragma once
#include "Globals.h"
#include "Sprite.h"

enum eEnemyType
{
	ENEMY_GOOMBA = 0,
	ENEMY_TURTLE
};

class EnemySpawnPoint : public Sprite
{
public:	
	EnemySpawnPoint(eEnemyType InEnemyType, int X, int Y);
	virtual void Tick(double DeltaTime);
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum = 0);
	virtual void SpawnEnemy();
	virtual void OnEnemyDestroyed();

protected:	
	bool bCanSpawnWhileVisible;
	bool bHasSpawnedEnemy;
	bool bInRange;
	eEnemyType EnemyType;

	eEnemyType GetEnemyTypeFromTileGID(int TileGID);
};

