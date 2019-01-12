#include "../inc/EnemySprite.h"
#include "../inc/Globals.h"
#include "../inc/SimpleSprites.h"
#include "../inc/PlayerSprite.h"
#include "../inc/ItemSprite.h"
#include "../inc/SpriteList.h"
#include "../inc/TMXMap.h"
#include "../inc/Game.h"

#define PI 3.14159265
#define MUSHROOM_COUNTDOWN 420

float EaseLerp(float time, float startValue, float change, float duration) {
	time /= duration / 2;
	if (time < 1) {
		return change / 2 * time * time + startValue;
	}

	time--;
	return -change / 2 * (time * (time - 2) - 1) + startValue;
};

float Ease2(float t) { return t*t*t*t*t; }

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

bool EnemySprite::IsFireable()
{
	return !IsDying();
}

bool GoombaEnemySprite::Interact(EnemySprite* Enemy)
{	
	if (!bPendingDelete && !Enemy->WasBricked())
	{
		VelocityX *= -1;
		Enemy->OnInteractedWith(this);
		//PosX += VelocityX;
		SetFlip(VelocityX < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}

	return true;
}

void GoombaEnemySprite::OnInteractedWith(EnemySprite* Other)
{
	VelocityX *= -1;
	SetFlip(VelocityX < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void GoombaEnemySprite::GetStomped()
{	
	VelocityX = 0;
	VelocityY = 0;
	DyingCount = 1;
	PlayAnimation(NULL);
	SetTexture(GResourceManager->EGoombaFlagTexture->Texture);
}

void EnemySprite::GetBricked(int TileX, int TileY)
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

void EnemySprite::GetStarred(int TileX, int TileY)
{
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

void EnemySprite::GetFired()
{
	GetBricked(PosX / 64, PosY / 64);
}

SDL_Rect EnemySprite::GetScreenSpaceCustomRect()
{
	return{ (int)PosX - (int)TheMap->GetScrollX(), ((int)PosY + Rect.h - 64)- (int)TheMap->GetScrollY(), Rect.w, 63};
}

GoombaEnemySprite::GoombaEnemySprite(EnemySpawnPoint* Spawner) :
	EnemySprite(Spawner)
{
	Sprite::Sprite(GResourceManager->GuyTexture->Texture);
	SetTexture(GResourceManager->GuyTexture->Texture);
	DyingCount = 0;
	CollisionRect = {14, 20, 40, 28};
	VelocityX = -2.1;

	if (TheMap->GetScrollVelocityX() < 0)
	{
		VelocityX = 2.1;
	}
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
		Flip = SDL_FLIP_VERTICAL;
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

	if (TheMap->GetScrollVelocityX() < 0)
	{
		VelocityX = 2.1;
		SetFlip(SDL_FLIP_NONE);
	}
	else
	{
		SetFlip(SDL_FLIP_HORIZONTAL);
	}

	PlayAnimation(GResourceManager->TurtleAnimation);
	SetWidth(64);
	SetHeight(128);	
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

void TurtleEnemySprite::OnInteractedWith(EnemySprite* Other)
{
	if (TurtleState == TURTLE_STATE_WALK)
	{
		VelocityX *= -1;
		SetFlip(VelocityX < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
}

GiantGoomba::GiantGoomba(EnemySpawnPoint* Spawner) :
	GoombaEnemySprite(Spawner)
{		

	LoadGiantTexture();
	SetTexture(Texture);	
	SetWidth(GIANT_GOOMBA_SIZE);
	SetHeight(GIANT_GOOMBA_SIZE);
	DestinationX = Spawner->GetPosX() + 64;
	JumpCount = 0;

	EnterState(GIANT_STATE_WALK_TO_START);	

	
	// TODO: Need custom collision using two rects at least	
	//VelocityX = 0;	
}

GiantGoomba::~GiantGoomba()
{
	//if (CurrentState >= GIANT_STATE_CHASE)
	{		
		SDL_DestroyTexture(Texture);		
	}
}

void GiantGoomba::Tick(double DeltaTime)
{
	if (ThePlayer->IsWarping())
	{
		return;
	}

	if (DyingCount == 0 && !bGotBricked)
	{
		PhysicsSprite::Tick(DeltaTime);
		HandleMovement();

		CountDown--;

		if (CurrentState == GIANT_STATE_WALK_TO_START)
		{
			if ((int)CountDown % 6 == 0)
			{
				if (Flip == SDL_FLIP_HORIZONTAL)
				{
					Flip = SDL_FLIP_NONE;
				}
				else
				{
					Flip = SDL_FLIP_HORIZONTAL;
				}
			}
			if (PosX <= DestinationX)
			{
				LeaveState(CurrentState);
				EnterState(GIANT_STATE_MUSHROOM);
			}
		}
		else if (CurrentState == GIANT_STATE_MUSHROOM)
		{
			if (CountDown == MUSHROOM_COUNTDOWN - 30 || CountDown == MUSHROOM_COUNTDOWN - 120 || CountDown == MUSHROOM_COUNTDOWN - 135 || CountDown == MUSHROOM_COUNTDOWN - 150 ||
				CountDown == MUSHROOM_COUNTDOWN - 160 || CountDown == MUSHROOM_COUNTDOWN - 133 || CountDown == MUSHROOM_COUNTDOWN - 138 ||
				CountDown == MUSHROOM_COUNTDOWN - 140 || CountDown == MUSHROOM_COUNTDOWN - 144 || CountDown == MUSHROOM_COUNTDOWN - 150 || CountDown == MUSHROOM_COUNTDOWN - 155 ||
				CountDown == MUSHROOM_COUNTDOWN - 157)
			{
				//double CurrentSize = 448 * Scale;			

				BigMushroomItemSprite* NewMushroom = new BigMushroomItemSprite(PosX + Rect.w / 2 - 32, TheMap->GetScrollY() - 64, false);
				NewMushroom->SetVelocityX(0);
				NewMushroom->SetVelocityY(0.15);
				ItemSprites.push_back(NewMushroom);
				Mix_PlayChannel(CHANNEL_FLAG_POLE, PowerUpSound, 0);
			}
			else if (CountDown == MUSHROOM_COUNTDOWN - 230 || CountDown == MUSHROOM_COUNTDOWN - 280)
			{
				JumpCount++;
				VelocityY = -20;
				Mix_PlayChannel(CHANNEL_JUMP, JumpSound, 0);
			}
			else if (CountDown == MUSHROOM_COUNTDOWN - 305)
			{
				Mix_PlayMusic(ChaseMusic, -1);
			}
			else if (CountDown == 0)
			{
				LeaveState(CurrentState);
				EnterState(GIANT_STATE_CHASE);
			}
		}
		else if (CurrentState == GIANT_STATE_CHASE)
		{
			if (CountDown == 0)
			{
				if (Flip == SDL_FLIP_HORIZONTAL)
				{
					Flip = SDL_FLIP_NONE;
				}
				else
				{
					Flip = SDL_FLIP_HORIZONTAL;
				}
				CountDown = 4;
			}
		}

		UpdateScale();
	}
	else if (bGotBricked)
	{
		PosX += VelocityX;
		PosY += VelocityY;
		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;
		VelocityY += BASE_FALL_VELOCITY / 2;
		Flip = SDL_FLIP_VERTICAL;
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


void GiantGoomba::EnterState(eGiantGoombaState NewState)
{
	switch (NewState)
	{
	case GIANT_STATE_WALK_TO_START:
		Mix_HaltMusic();
		VelocityX = -2;
		DyingCount = 0;
		CountDown = 120;
		Scale = 0.12;
		ScaleRate = 0.03;
		ScaleTarget = Scale;
		ScaleCountDown = 0;		
		break;

	case GIANT_STATE_MUSHROOM:
		VelocityX = 0;
		CountDown = MUSHROOM_COUNTDOWN;
		break;

	case GIANT_STATE_CHASE:				
		//PosY -= 448;
		CollisionRect = { 0, 0, GIANT_GOOMBA_SIZE, GIANT_GOOMBA_SIZE };
		VelocityX = -4.3;
		CountDown = 4;	
		ThePlayer->SetFrozen(false);
		TheMap->SetAutoScrollX(-4);	
		break;	
	default:
		break;
	}
	
	CurrentState = NewState;
}

void GiantGoomba::LeaveState(eGiantGoombaState PreviousState)
{
	switch (PreviousState)
	{
	case GIANT_STATE_MUSHROOM:
		break;

	default:
		break;
	}
}

void GiantGoomba::LoadGiantTexture()
{
	string FileName = "giantgoomba.bmp";
	AnimData.Anim = NULL;
	SDL_Surface * Image = SDL_LoadBMP((TEXTURE_PATH + FileName).c_str());
	SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0, 0xFF));
	SDL_Log("Loaded: %d", Image);

	Texture = SDL_CreateTextureFromSurface(GetRenderer(), Image);

	SDL_FreeSurface(Image);
	
}

SDL_Rect GiantGoomba::GetScreenSpaceCustomRect()
{
	//double CurrentSize = GIANT_GOOMBA_SIZE * Scale;
	//SDL_Rect NewRect = { (Rect.x - TheMap->GetScrollX()) + Rect.w / 2 - CurrentSize / 2  , (Rect.y - TheMap->GetScrollY()) + GIANT_GOOMBA_SIZE - CurrentSize - 1, CurrentSize, CurrentSize };
	//
	//return NewRect;
	

	return GetScreenSpaceCollisionRect();
}

SDL_Rect GiantGoomba::GetScreenSpaceCollisionRect()
{
	double CurrentSize = GIANT_GOOMBA_SIZE * Scale;
	SDL_Rect NewRect = { (Rect.x - TheMap->GetScrollX()) + Rect.w / 2 - CurrentSize / 2  , (Rect.y - TheMap->GetScrollY()) + GIANT_GOOMBA_SIZE - CurrentSize - 1, CurrentSize, CurrentSize };

	return NewRect;
}
void GiantGoomba::Render(SDL_Renderer* Renderer, int ResourceNum)
{
	SDL_Rect RealRect = Rect;
	double CurrentSize = GIANT_GOOMBA_SIZE * Scale;

	Rect.x = Rect.x + Rect.w / 2 - CurrentSize / 2;
	Rect.y = Rect.y + (GIANT_GOOMBA_SIZE - CurrentSize);

	EnemySprite::Render(Renderer, ResourceNum);
	Rect = RealRect;

	if (bRenderCollision)
	{
		RenderCollision(GRenderer);
	}	
}

bool GiantGoomba::InMapWindow(SDL_Point Offset)
{

	return true;

	SDL_Rect MapWindow = TheMap->GetVisibleWindow();

	// To the right of the window
	return (PosX + Rect.w >= MapWindow.x - Offset.x && PosX <= MapWindow.x + MapWindow.w + Offset.x &&
		PosY + Rect.h >= MapWindow.y - Offset.y && PosY <= MapWindow.y + MapWindow.h + Offset.y);
}

void GiantGoomba::UpdateScale()
{
	if (ScaleCountDown == 0)
	{
		if (Scale < ScaleTarget)
		{
			Scale += ScaleRate;

			if (Scale > ScaleTarget)
			{
				ScaleCountDown = 30;				
			}
		}
		else if (Scale > ScaleTarget)
		{
			Scale -= ScaleRate;

			if (Scale < ScaleTarget)
			{
				ScaleCountDown = 30;
			}
		}
	}
	else
	{
		ScaleCountDown--;

		Scale += ScaleRate * 0.45;

		if (ScaleCountDown % 5 == 0)
		{
			ScaleRate *= -1;
		}
		if (ScaleCountDown == 0)
		{			
			Scale = ScaleTarget;
		}
	}
}

void GiantGoomba::Interact(ItemSprite* Item)
{

	if (CurrentState == GIANT_STATE_MUSHROOM)
	{
		Item->Delete();
		ScaleTarget += 0.08;
		ScaleRate += 0.02;
		if (ScaleTarget > 1)
		{
			ScaleTarget = 1;
		}
		Mix_PlayChannel(CHANNEL_POWER_UP, PowerUpGetSound, 0);
	}
}

bool GiantGoomba::Interact(EnemySprite* Enemy)
{
	Enemy->GetBricked(PosX / 64, PosY / 64);
	return false;
}

void GiantGoomba::HandleMovement()
{
	if (DyingCount == 0 && !bGotBricked)
	{		
		int NumPixelsToMove = round(fabs(VelocityX));
		SDL_Rect NewRect = GetScreenSpaceCustomRect();

		for (int x = 0; x < NumPixelsToMove; x++)
		{
			NewRect.x += VelocityX > 0 ? 1 : -1;

			vector<TileInfo> Tiles;

			if (TheMap->CheckCollision(NewRect, Tiles, true, true))
			{
				for (int i = 0; i < Tiles.size(); i++)
				{
					if (TheMap->IsCollidableTile(Tiles[i].MetaTileType, Tiles[i].Location.x, Tiles[i].Location.y))
					{
						TheMap->DoBrickBreak(Tiles[i].Location.x, Tiles[i].Location.y);

						/*int SpawnX = Tiles[i].Location.x * 64;
						int SpawnY = Tiles[i].Location.y * 64;
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY - 32, -4));
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY - 32, 4));
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY + 32, -4));
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY + 32, 4));

						Mix_PlayChannel(CHANNEL_BREAK_BRICK, BreakBrickSound, 0);
						Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);

						TheMap->SetForegroundTile(Tiles[i].Location.x, Tiles[i].Location.y, -1);						
						TheMap->SetMetaLayerTile(Tiles[i].Location.x, Tiles[i].Location.y, TILE_NONE);*/
					}
				}
				//break;
			}
			else
			{
				PosX += VelocityX > 0 ? 1 : -1;
				CheckCollisionWithSprites();
			}
		}

		bool bIsInAir = !IsOnGround();

		if (CurrentState == GIANT_STATE_MUSHROOM)
		{
			
			if (bIsInAir)
			{
				VelocityY += BASE_FALL_VELOCITY;				
			}

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

		// If we've landed, break bricks
		if (CurrentState == GIANT_STATE_MUSHROOM && bIsInAir && IsOnGround())
		{
			bIsInAir = false;
			Mix_PlayChannel(CHANNEL_FIRE_BALL, FireworkSound, 0);
			Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
			if (JumpCount == 1)
			{
				vector<TileInfo> Tiles;

				if (TheMap->CheckCollision(NewRect, Tiles, true, true))
				{
					for (int i = 0; i < Tiles.size(); i++)
					{
						if (Tiles[i].MetaTileType == TILE_COIN)
						{
							TheMap->SetForegroundTile(Tiles[i].Location.x, Tiles[i].Location.y, -1);
							TheMap->SetMetaLayerTile(Tiles[i].Location.x, Tiles[i].Location.y, TILE_NONE);
							TheMap->SetBackgroundTile(Tiles[i].Location.x, Tiles[i].Location.y, -1);

							int SpawnX = Tiles[i].Location.x * 64;
							int SpawnY = Tiles[i].Location.y * 64;
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY - 32, -4));
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY - 32, 4));
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY + 32, -4));
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY + 32, 4));

							Mix_PlayChannel(CHANNEL_BREAK_BRICK, BreakBrickSound, 0);
						}
					}
				}
			}
			else if (JumpCount == 2)
			{
				vector<TileInfo> Tiles;

				if (TheMap->CheckCollision(NewRect, Tiles, true, true))
				{
					for (int i = 0; i < Tiles.size(); i++)
					{
						if (Tiles[i].MetaTileType == TILE_BREAK_ON_TOUCH)
						{
							TheMap->SetForegroundTile(Tiles[i].Location.x, Tiles[i].Location.y, -1);
							TheMap->SetMetaLayerTile(Tiles[i].Location.x, Tiles[i].Location.y, TILE_NONE);
							TheMap->SetBackgroundTile(Tiles[i].Location.x, Tiles[i].Location.y, -1);

							int SpawnX = Tiles[i].Location.x * 64;
							int SpawnY = Tiles[i].Location.y * 64;
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY - 32, -4));
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY - 32, 4));
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY + 32, -4));
							SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY + 32, 4));

							Mix_PlayChannel(CHANNEL_BREAK_BRICK, BreakBrickSound, 0);
						}
					}
				}
			}
		}		
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

void GiantGoomba::GetStomped()
{
	ThePlayer->SetVelocityY(-70);
	ThePlayer->SetVelocityX(-30);
	ThePlayer->SetPosition(ThePlayer->GetPosX(), ThePlayer->GetPosY() - 32);

	Mix_PlayChannel(CHANNEL_STOMP, StompSound, 0);
}

void GiantGoomba::Interact(Sprite *OtherSprite)
{
	OtherSprite->Delete();
}

void GiantGoomba::GetFired()
{
	Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
}

void GiantGoomba::GetBricked(int TileX, int TileY)
{
	GoombaEnemySprite::GetBricked(TileX, TileY);
	TheMap->SetAutoScrollX(0, false);

	// TODO: If the player doesn't have a star, play the music
	Mix_PlayMusic(BGMusic, -1);
}

GiantOkto::GiantOkto(int X, int Y) :
	EnemySprite(nullptr)
{		
	SetPosition(X, Y);
	SetTexture(GResourceManager->OktoEyesTexture->Texture);	
	SetWidth(128);
	SetHeight(128);	
	
	EnterState(GOS_FADE_IN_EYES);	

	SDL_SetTextureBlendMode(Texture, SDL_BLENDMODE_BLEND);	
	SDL_SetTextureAlphaMod(Texture, 0);
	CollisionRect = { 28, 15, 72, 64 };
	// TODO: Need custom collision using two rects at least	
	
}

GiantOkto::~GiantOkto()
{
	//if (CurrentState >= GIANT_STATE_CHASE)
	{		
		SDL_DestroyTexture(Texture);		
	}
}

void GiantOkto::Tick(double DeltaTime)
{
	if (ThePlayer->IsWarping())
	{
		return;
	}
	
	if (DyingCount == 0 && !bGotBricked)
	{
		PhysicsSprite::Tick(DeltaTime);
		HandleMovement();

		
		CountDown--;

		if (CurrentState == GOS_FADE_IN_EYES)
		{
			float Alpha = EaseLerp(OKTO_FADE_IN - CountDown, 0, 255, OKTO_FADE_IN);
			Alpha = 255 * Ease2((OKTO_FADE_IN - CountDown) / OKTO_FADE_IN);
			SDL_SetTextureAlphaMod(Texture, round(Alpha));

			if (CountDown <= 0)
			{
				LeaveState(GOS_FADE_IN_EYES);
				EnterState(GOS_FADE_IN_BODY);
			}
		}
		else if (CurrentState == GIANT_STATE_MUSHROOM)
		{
			float Alpha = EaseLerp(OKTO_FADE_IN - CountDown, 0, 255, OKTO_FADE_IN);
			Alpha = 255 * Ease2((OKTO_FADE_IN - CountDown) / OKTO_FADE_IN);
			SDL_SetTextureAlphaMod(Texture, round(Alpha));

			if (CountDown <= 0)
			{
				LeaveState(GOS_FADE_IN_BODY);
				EnterState(GOS_JUMP_BACK);
			}
		}
		else if (CurrentState == GOS_JUMP_BACK)
		{

			if (CountDown == 50)
			{
				SetAnimationPlayRate(2);
			}
			else  if (CountDown == 12)
			{
				VelocityY = -26;				
			}
			else if (CountDown == 0)
			{
				VelocityY = 0;
				SetAnimationPlayRate(1);
			}
		}
	}
	else if (bGotBricked)
	{
		PosX += VelocityX;
		PosY += VelocityY;
		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;
		VelocityY += BASE_FALL_VELOCITY / 2;
		Flip = SDL_FLIP_VERTICAL;
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


void GiantOkto::EnterState(eGiantOktoState NewState)
{
	switch (NewState)
	{
	case GOS_FADE_IN_EYES:
		Mix_HaltMusic();		
		DyingCount = 0;
		CountDown = OKTO_FADE_IN;
		ScaleCountDown = 0;		
		break;

	case GOS_FADE_IN_BODY:
		DyingCount = 0;
		CountDown = OKTO_FADE_IN;
		ScaleCountDown = 0;

		SetTexture(GResourceManager->OktoTexture->Texture);
		SDL_SetTextureBlendMode(Texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(Texture, 0);
		break;

	case GOS_JUMP_BACK:						
		CountDown = 90;	
		SetAnimationPlayRate(1);			
		break;	
	default:
		break;
	}
	
	CurrentState = NewState;
}

void GiantOkto::LeaveState(eGiantOktoState PreviousState)
{
	switch (PreviousState)
	{
	case GIANT_STATE_MUSHROOM:
		break;

	default:
		break;
	}
}


void GiantOkto::Render(SDL_Renderer* Renderer, int ResourceNum)
{
	if (CurrentState == GOS_FADE_IN_BODY)
	{
		SetTexture(GResourceManager->OktoEyesTexture->Texture);
		EnemySprite::Render(Renderer, ResourceNum);

		PlayAnimation(GResourceManager->OktoAnimation);
		SetAnimationPlayRate(0);
	}

	EnemySprite::Render(Renderer, ResourceNum);
}

void GiantOkto::HandleMovement()
{
	if (DyingCount == 0 && !bGotBricked)
	{		
		int NumPixelsToMove = round(fabs(VelocityY));
		SDL_Rect NewRect = GetScreenSpaceCustomRect();

		for (int x = 0; x < NumPixelsToMove; x++)
		{
			NewRect.y += VelocityY > 0 ? 1 : -1;

			PosY += VelocityY > 0 ? 1 : -1;
			CheckCollisionWithSprites();			
		}

		bool bIsInAir = !IsOnGround();

		if (CurrentState == GIANT_STATE_MUSHROOM)
		{
			
			if (bIsInAir)
			{
				//VelocityY += BASE_FALL_VELOCITY;				
			}

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
			VelocityX = 0;
		}
		
		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;

		// If we've landed, break bricks
		if (CurrentState == GIANT_STATE_MUSHROOM && bIsInAir && IsOnGround())
		{
			bIsInAir = false;
			Mix_PlayChannel(CHANNEL_FIRE_BALL, FireworkSound, 0);
			Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
						
		}		
	}
	else if (bGotBricked)
	{
		PosX += VelocityX;
		PosY += VelocityY;
		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;
		//VelocityY += BASE_FALL_VELOCITY / 2;

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


PlantEnemySprite::PlantEnemySprite(EnemySpawnPoint* Spawner) :
	EnemySprite(Spawner)
{
	Sprite::Sprite(GResourceManager->PlantTexture->Texture);

	DyingCount = 0;
	CollisionRect = { 10, 50, 48, 44 };
	
	RenderLayer = RENDER_LAYER_BEHIND_FG;
	
	CurrentState = PLANT_STATE_RETRACTED;
	PlayAnimation(GResourceManager->PlantAnimation);
	SetPosition(Spawner->GetPosX(), Spawner->GetPosY() - 64);
	EnterState(PLANT_STATE_GOING_UP);
}

void PlantEnemySprite::EnterState(ePlantState NewState)
{

	// Don't do anything if we're trying to go up and the player is too close
	bool bPlayerTooClose = PosX - (ThePlayer->GetPosX() + ThePlayer->GetWidth()) <= 96 &&
		                   ThePlayer->GetPosX() - (PosX + Rect.w) <= 96;

	if (NewState == PLANT_STATE_GOING_UP && bPlayerTooClose)
	{

		return;
	}

	if (NewState > PLANT_STATE_GOING_DOWN)
	{
		NewState = PLANT_STATE_RETRACTED;
	}

	switch (NewState)
	{
	case PLANT_STATE_RETRACTED:
		CountDown = 62;
		VelocityY = 0;
		break;

	case PLANT_STATE_GOING_UP:
		CountDown = 62;
		VelocityY = -2;
		break;

	case PLANT_STATE_EXTENDED:
		CountDown = 62;
		VelocityY = 0;
		break;

	case PLANT_STATE_GOING_DOWN:
		CountDown = 62;
		VelocityY = 2;
		break;

	default:
		break;
	}
	CurrentState = NewState;
}
void PlantEnemySprite::LeaveState(ePlantState PreviousState)
{

}

void PlantEnemySprite::Tick(double DeltaTime)
{
	Sprite::Tick(DeltaTime);

	if (DyingCount == 0 && !bGotBricked)
	{
		PosX += VelocityX;
		PosY += VelocityY;

		Rect.x = PosX;
		Rect.y = PosY;

		CountDown--;

		if (CountDown <= 0)
		{
			LeaveState(CurrentState);
			EnterState((ePlantState)(CurrentState + 1));
		}
	}
	else if (bGotBricked)
	{
		PosX += VelocityX;
		PosY += VelocityY;
		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;
		VelocityY += BASE_FALL_VELOCITY / 2;
		Flip = SDL_FLIP_VERTICAL;
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

BulletEnemySprite::BulletEnemySprite(EnemySpawnPoint* Spawner) :
	EnemySprite(Spawner)
{
	SetTexture(GResourceManager->BulletTexture->Texture);
	SetWidth(64);
	SetHeight(64);

	if (ThePlayer->GetPosX() < Spawner->GetPosX())
	{
		VelocityX = -6;
		DestinationX = Spawner->GetPosX() - 64;
	}
	else
	{
		VelocityX = 6;
		SetFlip(SDL_FLIP_HORIZONTAL);
		DestinationX = Spawner->GetPosX() + 64;
	}

	Mix_PlayChannel(CHANNEL_FIREWORK, FireworkSound, 0);
	
	RenderLayer = RENDER_LAYER_BEHIND_FG;
	CollisionRect = { 9, 24, 44, 28 };
}

void BulletEnemySprite::Tick(double DeltaTime)
{	
	Sprite::Tick(DeltaTime);

	if (RenderLayer == RENDER_LAYER_BEHIND_FG && (VelocityX <= 0 && PosX < DestinationX || VelocityX > 0 && PosX > DestinationX))
	{
		RenderLayer = RENDER_LAYER_TOP;
	}

	if (bGotBricked)
	{
		VelocityY += BASE_FALL_VELOCITY / 4;

		if (VelocityY > 10)
		{
			VelocityY = 10;
		}
	}

	PosX += VelocityX;
	PosY += VelocityY;

	Rect.x = PosX;
	Rect.y = PosY;
}

void BulletEnemySprite::GetStomped()
{
	if (!IsDying())
	{
		VelocityX = 0;	
		PosY--;
			
		bGotBricked = true;
	}
}

SunEnemySprite::SunEnemySprite() : EnemySprite(NULL)
{
	RotationSpeed = 830;
	Radius = 48;
	PlayAnimation(GResourceManager->SunAnimation);
	SetWidth(128);
	SetHeight(128);
	bDrawScreenSpace = true;
	SetPosition(128, 128);
	Angle = 0;
	
	Center.x = 64;
	Center.y = 128;
	
	VelocityX = 0;
	VelocityY = 0;
	CountDown = 60;
	SetPosition(128, 128);
	Angle = -90;
	EnterState(Sun_WaitThenAttackRight);
}

void SunEnemySprite::Tick(double DeltaTime)
{	
	Sprite::Tick(DeltaTime);

	if (!IsDying())
	{
		if (SunState == Sun_WaitThenAttackRight)
		{
			//Angle += RotationSpeed * DeltaTime;
			//if (Angle >= 360)
			//{
			//	Angle = 0;
			//}
			//double OffsetX = -sin((Angle)*PI / 180) * Radius;
			//double OffsetY = cos((Angle)*PI / 180) * Radius;

			//VelocityX = OffsetX / 3.25;// +Center.x;
			//VelocityY = OffsetY / 3.25;// *1.65 + Center.y - 50;

			//if (CountDown <= 0 && Angle > 180)
			//{
			//	EnterState(Sun_WaitAndResetTimer20);
			//}

			Angle += RotationSpeed * DeltaTime;
			if (Angle >= 360)
			{
				Angle = 0;
			}
			double OffsetX = -sin((Angle + 90)*PI / 180) * Radius;
			double OffsetY = cos((Angle + 90)*PI / 180) * Radius;

			PosX = OffsetX + Center.x;
			PosY = OffsetY + Center.y;

			if (CountDown <= 0 && Angle == 0)
			{
				EnterState(Sun_WaitAndResetTimer20);
			}
		}
		else if (SunState == Sun_WaitThenAttackLeft)
		{
			//Angle -= RotationSpeed * DeltaTime;
			//if (Angle <= -360)
			//{
			//	Angle = 0;
			//}
			//double OffsetX = -sin(Angle*PI / 180) * Radius;
			//double OffsetY = cos(Angle*PI / 180) * Radius;

			//VelocityX = OffsetX / 3.25;// +Center.x;
			//VelocityY = OffsetY / 3.25;// *1.65 + Center.y - 50;

			Angle += RotationSpeed * DeltaTime;
			if (Angle >= 360)
			{
				Angle = 0;
			}
			double OffsetX = sin((Angle + 90)*PI / 180) * Radius;
			double OffsetY = cos((Angle + 90)*PI / 180) * Radius;

			PosX = OffsetX + Center.x;
			PosY = OffsetY + Center.y;

			if (CountDown <= 0 && Angle == 0)
			{
				EnterState(Sun_WaitAndResetTimer20Again);
			}
		}
		else if (SunState == Sun_WaitAndResetTimer20)
		{
			if (CountDown <= 0)
			{
				EnterState(Sun_WaitForUpperReturn);
			}
		}
		else if (SunState == Sun_WaitAndResetTimer20Again)
		{
			if (CountDown <= 0)
			{
				EnterState(Sun_WaitForUpperReturn2);
			}
		}
		else if (SunState == Sun_WaitForUpperReturn)
		{
			//VelocityY -= 0.25;
			PosX += 6.5;

			PosY = -0.0036490483539095 * (PosX * PosX) + 3.269547325102881 * PosX + 67.62139917695473;

			if (PosY < 119)
			{
				EnterState(Sun_WaitThenAttackLeft);
			}
		}
		else if (SunState == Sun_WaitForUpperReturn2)
		{
			PosX -= 6.5;

			PosY = -0.0036490483539095 * (PosX * PosX) + 3.269547325102881 * PosX + 67.62139917695473;

			if (PosY < 119)
			{
				EnterState(Sun_WaitThenAttackRight);
			}
		}
		PosX += VelocityX;
		PosY += VelocityY;
		Rect.x = PosX;
		Rect.y = PosY;

		/*if (Rect.y > 800 && VelocityY > 0)
		{
			VelocityY *= ;
			Velocity
		}*/

		CountDown--;
	}
	else
	{
		PosX += VelocityX;
		PosY += VelocityY;
		Rect.x = PosX;
		Rect.y = PosY;
	}
	
}

void SunEnemySprite::EnterState(eSunState NewState)
{
	switch (NewState)
	{

	case Sun_WaitForUpperReturn:
		VelocityX = 0;
		VelocityY = 0;
		/*PosX = 40;
		PosY = 54;*/
		/*PosX = 40;
		PosY = 54;*/
		//CountDown = 40;
		
		break;

	case Sun_WaitForUpperReturn2:
		VelocityX = 0;
		VelocityY = 0;
		/*PosX = 862;
		PosY = 54;*/
		//CountDown = 40;

		break;

	case Sun_WaitAndResetTimer20:
		VelocityX = 0;
		VelocityY = 0;		
		CountDown = 20;

		break;

	case Sun_WaitAndResetTimer20Again:
		VelocityX = 0;
		VelocityY = 0;
		
		CountDown = 20;

		break;

	case Sun_WaitThenAttackLeft:
		VelocityX = 0;
		VelocityY = 0;
		CountDown = 60;
		//SetPosition(862, 54);
		Angle = 0;
		Center.x = 832;
		Center.y = 128;
		
		break;

	case Sun_WaitThenAttackRight:
		VelocityX = 0;
		VelocityY = 0;
		CountDown = 60;
		//SetPosition(128, 54);
		Angle = 0;
		Center.x = 64;
		Center.y = 128;

		break;
	};
	Scale += 1;
	SunState = NewState;
}
void SunEnemySprite::LeaveState(eSunState PreviousState)
{

}

SDL_Rect SunEnemySprite::GetScreenSpaceCollisionRect()
{
	return{ Rect.x + 32, Rect.y + 32, 128 - 64, 128 - 64 };
}

bool SunEnemySprite::IsStompable()
{
	return false;
}

bool SunEnemySprite::IsFireable()
{
	return false;
}

void SunEnemySprite::GetBricked(int TileX, int TileY)
{
	// TODO: Move this shit to the enemy sprite!
	if (!IsDying())
	{
		Mix_PlayChannel(CHANNEL_KICK, KickSound, 0);
		VelocityY = 20;

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
		TheGame->HandleSpecialEvent(SPECIAL_EVENT_SUN);
	}
}

void SunEnemySprite::GetStarred(int TileX, int TileY)
{
	
}