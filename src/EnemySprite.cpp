#include "../inc/EnemySprite.h"
#include "../inc/Globals.h"
#include "../inc/SimpleSprites.h"
#include "../inc/PlayerSprite.h"
#include "../inc/ItemSprite.h"
#include "../inc/SpriteList.h"
#include "../inc/TMXMap.h"

#define MUSHROOM_COUNTDOWN 306

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
	return{ (int)PosX - (int)TheMap->GetScrollX(), ((int)PosY + Rect.h - 64)- (int)TheMap->GetScrollY(), Rect.w, 63};
}

GoombaEnemySprite::GoombaEnemySprite(EnemySpawnPoint* Spawner) :
	EnemySprite(Spawner)
{
	Sprite::Sprite(GResourceManager->EGoombaSpriteTexture->Texture);
	
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
	DestinationX = Spawner->GetPosX() + 112;

	EnterState(GIANT_STATE_WALK_TO_START);	

	
	// TODO: Need custom collision using two rects at least	
	//VelocityX = 0;	
}

GiantGoomba::~GiantGoomba()
{
	//if (CurrentState >= GIANT_STATE_CHASE)
	{		
		SDL_DestroyTexture(Texture);
		TheMap->SetAutoScrollX(0, false);
	}
}

void GiantGoomba::Tick(double DeltaTime)
{
	//GoombaEnemySprite::Tick(DeltaTime);

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
		if (CountDown == MUSHROOM_COUNTDOWN - 30 || CountDown == MUSHROOM_COUNTDOWN - 80 || CountDown == MUSHROOM_COUNTDOWN - 135 || CountDown == MUSHROOM_COUNTDOWN - 150 ||
			CountDown == MUSHROOM_COUNTDOWN - 160 || CountDown == MUSHROOM_COUNTDOWN - 133 || CountDown == MUSHROOM_COUNTDOWN - 138 || 
			CountDown == MUSHROOM_COUNTDOWN - 140 || CountDown == MUSHROOM_COUNTDOWN - 144 || CountDown == MUSHROOM_COUNTDOWN - 150 || CountDown == MUSHROOM_COUNTDOWN - 155)
		{
			//double CurrentSize = 448 * Scale;			

			BigMushroomItemSprite* NewMushroom = new BigMushroomItemSprite(PosX + Rect.w / 2 - 32, TheMap->GetScrollY() - 64, false);
			NewMushroom->SetVelocityX(0);
			NewMushroom->SetVelocityY(0.15);
			ItemSprites.push_back(NewMushroom);			
		}
		else if (CountDown == MUSHROOM_COUNTDOWN - 200 || CountDown == MUSHROOM_COUNTDOWN - 240)
		{
			VelocityY = -20;
			Mix_PlayChannel(CHANNEL_JUMP, JumpSound, 0);
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


void GiantGoomba::EnterState(eGiantGoombaState NewState)
{
	switch (NewState)
	{
	case GIANT_STATE_WALK_TO_START:
		Mix_HaltMusic();
		VelocityX = -2;
		DyingCount = 0;
		CountDown = 120;
		Scale = 0.2;
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
		VelocityX = -3.1;
		CountDown = 4;	
		ThePlayer->SetFrozen(false);
		TheMap->SetAutoScrollX(-3);
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
				ScaleCountDown = 20;				
			}
		}
		else if (Scale > ScaleTarget)
		{
			Scale -= ScaleRate;

			if (Scale < ScaleTarget)
			{
				ScaleCountDown = 20;
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
			if (Scale < 0.90)
			{
				//Mix_HaltChannel(CHANNEL_POWER_UP);
			}
			Scale = ScaleTarget;
		}
	}
}

void GiantGoomba::Interact(ItemSprite* Item)
{
	Item->Delete();
	ScaleTarget += 0.091;

	if (ScaleTarget > 1)
	{
		ScaleTarget = 1;
	}
	Mix_PlayChannel(CHANNEL_POWER_UP, PowerUpGetSound, 0);
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
						int SpawnX = Tiles[i].Location.x * 64;
						int SpawnY = Tiles[i].Location.y * 64;
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY - 32, -4));
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY - 32, 4));
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY + 32, -4));
						SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY + 32, 4));

						Mix_PlayChannel(CHANNEL_BREAK_BRICK, BreakBrickSound, 0);

						TheMap->SetForegroundTile(Tiles[i].Location.x, Tiles[i].Location.y, -1);
						TheMap->SetMetaLayerTile(Tiles[i].Location.x, Tiles[i].Location.y, TILE_NONE);
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


		if (CurrentState == GIANT_STATE_MUSHROOM)
		{
			if (!IsOnGround())
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
	ThePlayer->SetVelocityY(-40);
	ThePlayer->SetVelocityX(-30);
	ThePlayer->SetPosition(ThePlayer->GetPosX(), ThePlayer->GetPosY() - 32);

	Mix_PlayChannel(CHANNEL_STOMP, StompSound, 0);
}

void GiantGoomba::Interact(Sprite *OtherSprite)
{
	OtherSprite->Delete();
}