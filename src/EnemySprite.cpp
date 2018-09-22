#include "../inc/EnemySprite.h"
#include "../inc/Globals.h"
#include "../inc/PlayerSprite.h"
#include "../inc/SpriteList.h"
#include "../inc/TMXMap.h"

EnemySprite::EnemySprite(EnemySpawnPoint* Spawner)
{
	bGotBricked = false;
	DyingCount = 0;
	MySpawner = Spawner;	
}

EnemySprite::~EnemySprite()
{
	if (MySpawner != NULL)
	{
		MySpawner->OnEnemyDestroyed();
	}
}

bool EnemySprite::IsDying()
{
	return bPendingDelete || DyingCount != 0 || bGotBricked;
}

bool EnemySprite::WasBricked()
{
	return bGotBricked;
}

bool EnemySprite::IsStompable()
{
	return !IsDying();
}

bool GoombaEnemySprite::Interact(EnemySprite* Enemy)
{	
	if (!bPendingDelete && !Enemy->WasBricked())
	{
		VelocityX *= -1;
		PosX += VelocityX;
		SetFlip(VelocityX < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}

	return true;
}

void GoombaEnemySprite::GetStomped()
{	
	VelocityX = 0;
	VelocityY = 0;
	DyingCount = 1;
	PlayAnimation(NULL);
	SetTexture(GResourceManager->EGoombaFlagTexture->Texture);
}

void GoombaEnemySprite::GetBricked(int TileX, int TileY)
{
	// TODO: Move this shit to the enemy sprite!
	if (!IsDying())
	{
		Mix_PlayChannel(CHANNEL_KICK, KickSound, 0);
		VelocityY = -10;
		
		// Which half of a tile are we on?
		SDL_Rect MapSpaceCollisionRect = GetMapSpaceCollisionRect();		
		
		if ((MapSpaceCollisionRect.x) / 64 * 64 < TileX)
		{
			VelocityX = -3;
		}
		else
		{
			VelocityX = 3;
		}
		bGotBricked = true;
		Flip = SDL_FLIP_VERTICAL;
	}
}

void GoombaEnemySprite::GetFired()
{
	GetBricked(PosX / 64, PosY / 64);
}

SDL_Rect EnemySprite::GetScreenSpaceCustomRect()
{
	return{ (int)PosX - (int)TheMap->GetScrollX(), (int)PosY - (int)TheMap->GetScrollY(), Rect.w, Rect.h - 1};
}

GoombaEnemySprite::GoombaEnemySprite(EnemySpawnPoint* Spawner) :
	EnemySprite(Spawner)
{
	Sprite::Sprite(GResourceManager->EGoombaSpriteTexture->Texture);
	
	DyingCount = 0;
	CollisionRect = {14, 20, 40, 28};
	VelocityX = -2.1;
	PlayAnimation(GResourceManager->EGoombaAnimation);
}

void GoombaEnemySprite::Tick(double DeltaTime)
{	
	if (DyingCount == 0 && !bGotBricked)
	{
		PhysicsSprite::Tick(DeltaTime);

		int NumPixelsToMove = round(fabs(VelocityX));
		SDL_Rect NewRect = GetScreenSpaceCustomRect();

		for (int x = 0; x < NumPixelsToMove; x++)
		{			
			NewRect.x += VelocityX > 0 ? 1 : -1;

			vector<TileInfo> Tiles;

			if (TheMap->CheckCollision(NewRect, Tiles, true, true))
			{				
				VelocityX *= -1; 
				HandleTileCollision(Tiles);
				SetFlip(VelocityX < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
				break;				
			}
			else
			{
				PosX += VelocityX > 0 ? 1 : -1;		
				CheckCollisionWithSprites();
			}
		}


		if (!IsOnGround())
		{
			VelocityY += BASE_FALL_VELOCITY;

			NumPixelsToMove = round(fabs(VelocityY));
			NewRect = GetScreenSpaceCustomRect();

			for (int y = 0; y < NumPixelsToMove; y++)
			{
				NewRect.y += VelocityY > 0 ? 1 : -1;

				if (!TheMap->CheckCollision(NewRect, true))
				{
					PosY += VelocityY > 0 ? 1 : -1;			
					CheckCollisionWithSprites();
				}
			}
		}
		else
		{
			VelocityY = 0;
		}

		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;
	}
	else if (bGotBricked)
	{
		PosX += VelocityX;
		PosY += VelocityY;
		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;
		VelocityY += BASE_FALL_VELOCITY / 2;

		if (PosY >= TheMap->GetHeightInPixels())
		{
			bPendingDelete = true;
		}

	}
	else
	{
		DyingCount++;

		if (DyingCount == DYING_COUNT)
		{
			bPendingDelete = true;
		}
	}
	
}

TurtleEnemySprite::TurtleEnemySprite(EnemySpawnPoint* Spawner) :
	GoombaEnemySprite(Spawner)
{
	Sprite::Sprite(GResourceManager->TurtleTexture->Texture);

	KillCount = 0;
	StompCooldown = 0;
	TurtleState = TURTLE_STATE_WALK;
	DyingCount = 0;
	CollisionRect = { 6, 80, 56, 46 };
	VelocityX = -2.1;
	PlayAnimation(GResourceManager->TurtleAnimation);
	SetWidth(64);
	SetHeight(128);
	SetFlip(SDL_FLIP_HORIZONTAL);
}


void TurtleEnemySprite::GetStomped()
{	
	StompCooldown = STOMP_COOL_DOWN;

	if (TurtleState != TURTLE_STATE_HIDE)
	{
		KillCount = 0;
		VelocityX = 0;
		VelocityY = 0;
		PlayAnimation(NULL);
		SetTexture(GResourceManager->TurtleShellTexture->Texture);
		SetWidth(64);
		SetHeight(128);
		TurtleState = TURTLE_STATE_HIDE;
	}
}

bool TurtleEnemySprite::IsStompable()
{
	return EnemySprite::IsStompable() && TurtleState != TURTLE_STATE_HIDE && StompCooldown == 0;
}

bool TurtleEnemySprite::IsKickable()
{
	return !IsDying() && TurtleState == TURTLE_STATE_HIDE && StompCooldown == 0;
}

void TurtleEnemySprite::GetKicked(PlayerSprite* Player)
{
	StompCooldown = STOMP_COOL_DOWN;

	if (Player->GetPosX() < PosX)
	{
		VelocityX = 14;
	}
	else
	{
		VelocityX = -14;
	}
	TurtleState = TURTLE_STATE_SHELL_SLIDE;
}

bool TurtleEnemySprite::Interact(EnemySprite* Enemy)
{
	if (TurtleState == TURTLE_STATE_WALK)
	{
		return GoombaEnemySprite::Interact(Enemy);
	}
	else if (!Enemy->IsDying() &&  TurtleState == TURTLE_STATE_SHELL_SLIDE)
	{
		int bIsOneUp = false;
		KillCount++;

		if (KillCount > 9)
		{
			KillCount = 9;
			bIsOneUp = true;
		}
		ThePlayer->AddScore(StompPoints[KillCount], Enemy->GetPosX(), Enemy->GetPosY(), bIsOneUp);	
		Enemy->GetBricked(PosX / 64 * 64, PosY / 64 * 64);
	}

	return true;
}

void TurtleEnemySprite::Tick(double DeltaTime)
{
	// We'll need to play the bump sound if we change directions after tick
	bool VelocityWasPositive = VelocityX >= 0;

	GoombaEnemySprite::Tick(DeltaTime);	

	if (StompCooldown > 0)
	{
		StompCooldown--;
	}

	// TODO: Break bricks or trigger item/coin metatiles

	// Play the bump sound if we changed direction 
	if (TurtleState == TURTLE_STATE_SHELL_SLIDE && (VelocityWasPositive && VelocityX < 0 || !VelocityWasPositive && VelocityX > 0))
	{
		Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
	}
}

bool TurtleEnemySprite::IsInteractable()
{
	return !IsDying() && StompCooldown == 0;
}

void TurtleEnemySprite::HandleTileCollision(vector<TileInfo> CollisionTiles)
{
	if (TurtleState == TURTLE_STATE_SHELL_SLIDE)
	{
		for (int i = 0; i < CollisionTiles.size(); i++)
		{
			TheMap->HandleCollision(CollisionTiles[i].Location.x, CollisionTiles[i].Location.y, true);
		}
	}
}