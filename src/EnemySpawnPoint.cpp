#include "../inc/EnemySpawnPoint.h"
#include "../inc/EnemySprite.h"
#include "../inc/SpriteList.h"

EnemySpawnPoint::EnemySpawnPoint(eEnemyType InEnemyType, int X, int Y)
{	
	bRespawn = true;
	bCanSpawnWhileVisible = false;
	bHasSpawnedEnemy = false;
	bInRange = false;
	//EnemyType = GetEnemyTypeFromTileGID(TileGID);
	EnemyType = InEnemyType;

	if (EnemyType == ENEMY_GOOMBA)
	{
		SetTexture(GResourceManager->EGoombaSpriteTexture->Texture);
	}
	if (EnemyType == ENEMY_TURTLE)
	{
		SetTexture(GResourceManager->TurtleTexture->Texture);
	}
	if (EnemyType == ENEMY_PLANT)
	{
		bCanSpawnWhileVisible = true;
		SetTexture(GResourceManager->PlantTexture->Texture);
	}
	if (EnemyType == ENEMY_GIANT_GOOMBA)
	{
		bCanSpawnWhileVisible = true;
		bRespawn = false;
	}
	if (EnemyType == ENEMY_BULLET)
	{
		bCanSpawnWhileVisible = true;
		bRespawn = true;		
		SetTexture(GResourceManager->BulletTexture->Texture);
	}
	SetWidth(64);
	SetHeight(64);
	SetPosition(X, Y);
}

eEnemyType EnemySpawnPoint::GetEnemyTypeFromTileGID(int TileGID)
{
	switch (TileGID)
	{
	case 19:
		return ENEMY_GOOMBA;
	case 29:
		return ENEMY_TURTLE;
	}

	return ENEMY_GOOMBA;
}

void EnemySpawnPoint::Tick(double DeltaTime)
{	
	bool bVisibleSpawnCheckResult = bCanSpawnWhileVisible || !InMapWindow();

	if (bVisibleSpawnCheckResult && InMapWindow({ SPAWN_DISTANCE, SPAWN_DISTANCE }))
	{
		if (!bHasSpawnedEnemy && !bInRange)
		{
			SpawnEnemy();

		}
		bInRange = true;
	}
	else
	{
		bInRange = false;
	}	
}

void EnemySpawnPoint::Render(SDL_Renderer* Renderer, int ResourceNum)
{
	if (bRenderCollision)
	{
		Sprite::Render(Renderer, ResourceNum);
	}
}

void EnemySpawnPoint::SpawnEnemy()
{
	bHasSpawnedEnemy = true;

	if (EnemyType == ENEMY_GOOMBA)
	{
		EnemySprite* NewEnemy = new GoombaEnemySprite(this);		
		NewEnemy->SetPosition(PosX, PosY);
		NewEnemy->SetWidth(64);
		NewEnemy->SetHeight(64);
		EnemySprites.push_back(NewEnemy);
	}
	else if (EnemyType == ENEMY_TURTLE)
	{
		EnemySprite* NewEnemy = new TurtleEnemySprite(this);
		NewEnemy->SetPosition(PosX, PosY - 64);
		NewEnemy->SetWidth(64);
		NewEnemy->SetHeight(128);
		EnemySprites.push_back(NewEnemy);
	}
	else if (EnemyType == ENEMY_GIANT_GOOMBA)
	{
		EnemySprite* NewEnemy = new GiantGoomba(this);
		NewEnemy->SetPosition(PosX + 340, PosY - GIANT_GOOMBA_SIZE + 65);
		EnemySprites.push_back(NewEnemy);
	}
	else if (EnemyType == ENEMY_PLANT)
	{
		PlantEnemySprite* NewEnemy = new PlantEnemySprite(this);
		NewEnemy->SetPosition(PosX, PosY - 64);
		NewEnemy->SetWidth(64);
		NewEnemy->SetHeight(128);
		EnemySprites.push_back(NewEnemy);
	}
	else if (EnemyType == ENEMY_BULLET)
	{
		BulletEnemySprite* NewEnemy = new BulletEnemySprite(this);
		NewEnemy->SetPosition(PosX, PosY);
		NewEnemy->SetWidth(64);
		NewEnemy->SetHeight(64);
		EnemySprites.push_back(NewEnemy);		
	}
}

void EnemySpawnPoint::OnEnemyDestroyed()
{
	if (bRespawn)
	{
		bHasSpawnedEnemy = false;
	}
}

TimedEnemySpawnPoint::TimedEnemySpawnPoint(eEnemyType InEnemyType, int X, int Y)
	: EnemySpawnPoint(InEnemyType, X, Y)
{	
	if (EnemyType == ENEMY_BULLET)
	{
		NumRespawnFrames = 200;
		RespawnCountDown = NumRespawnFrames;
	}
}

void TimedEnemySpawnPoint::Tick(double DeltaTime)
{
	bool bVisibleSpawnCheckResult = bCanSpawnWhileVisible || !InMapWindow();
	RespawnCountDown--;
	
	if (RespawnCountDown == 0)
	{
		bool bPlayerTooClose = PosX - (ThePlayer->GetPosX() + ThePlayer->GetWidth()) <= 96 &&
			ThePlayer->GetPosX() - (PosX + Rect.w) <= 96;

		if (!bPlayerTooClose && InMapWindow({ SPAWN_DISTANCE, SPAWN_DISTANCE }))
		{
			SpawnEnemy();			
		}

		RespawnCountDown = NumRespawnFrames;
	}
}