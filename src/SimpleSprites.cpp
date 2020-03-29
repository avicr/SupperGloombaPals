#include "../inc/SimpleSprites.h"
#include "../inc/TMXMap.h"
#include "../inc/PlayerSprite.h"
#include "../inc/EnemySprite.h"
#include "../inc/Game.h"
#include "../inc/SpriteList.h"

CoinEffectSprite::CoinEffectSprite(int X, int Y, eCoinType InCoinType)
{	
	CoinType = InCoinType;
	CountDown = 0;
	if (CoinType == COIN_RED)
	{
		PlayAnimation(GResourceManager->RedCoinEffectAnimation);
		VelocityY = -17;
		EndPosY = Y - 1500;
		FallVelocity = BASE_FALL_VELOCITY / 2;
		CountDown = 40;
	}
	else
	{
		EndPosY = Y - 74;
		PlayAnimation(GResourceManager->CoinEffectAnimation);
		VelocityY = -28;
		FallVelocity = BASE_FALL_VELOCITY;
	}
	SetPosition(X, Y);
	SetWidth(64);
	SetHeight(64);
	
}

void CoinEffectSprite::Tick(double DeltaTime)
{
	Sprite::Tick(DeltaTime);

	PosY += VelocityY;
	Rect.y = PosY;

	VelocityY += FallVelocity;	

	if (CoinType == COIN_NORMAL)
	{
		// Destroy the coin when we get back to the starting place
		if (PosY >= EndPosY && VelocityY > 0)
		{
			bPendingDelete = true;
			ThePlayer->AddScore(200, PosX, PosY);
		}
	}
	else
	{
		FallVelocity -= 0.05;
		if (FallVelocity < 0)
		{
			FallVelocity = 0;
			VelocityY = 0;
		}
		CountDown--;

		if (CountDown <= 0)
		{
			bPendingDelete = true;
		}
	}
}

BrickBreakSprite::BrickBreakSprite(int X, int Y, double InVelocityX, eTileMetaType MetaTileType)
{	
	eBrickBreakTilesetID BrickTilesetID = TheMap->GetBrickTilesetID();
	SetPosition(X, Y);
	SetWidth(64);
	SetHeight(64);
	
	VelocityY = -16;	
	VelocityX = InVelocityX;

	if (ThePlayer->GetTripLevel() == 0)
	{
 		eBrickBreakTilesetID BricktileSet = TheMap->GetBrickTilesetID();
		
		if (!TheMap->IsDestroyableByFireTile(MetaTileType))
		{
			switch (BricktileSet)
			{
			case BRICK_TILESET_UNDERGROUND:
				PlayAnimation(GResourceManager->BrickBreakUAnimation);
				break;
			default:
				PlayAnimation(GResourceManager->BrickBreakAnimation);
			}			
		}
		else
		{
			PlayAnimation(GResourceManager->IceBreakAnimation);
		}
	}
	else
	{
		SetTexture(GResourceManager->MagicMushroomTexture->Texture);
		Scale = 0.5;				
	}
	
}

void BrickBreakSprite::Tick(double DeltaTime)
{
	double OldVelocityX = VelocityX;
	Sprite::Tick(DeltaTime);

	PosX += VelocityX;
	PosY += VelocityY;
	Rect.x = PosX;
	Rect.y = PosY;

	if (VelocityX != 0)
	{
		VelocityX += VelocityX >= 0 ? -0.05 : 0.05;

		// If the sign changed, stop moving the x velocity
		if (VelocityX < 0 != VelocityX < 0)
		{
			VelocityX = 0;
		}
	}
	VelocityY += 0.75;

	if (ThePlayer->GetTripLevel() >= 2 && IsOnGround())
	{
		VelocityY = -15;
		VelocityX += rand() % 10;
		VelocityX *= (rand() % 2) == 0 ? -1 : 1;
	}

}

FireBallSprite::FireBallSprite(int X, int Y, double InVelocityX)
{
	SetPosition(X, Y);
	SetWidth(32);
	SetHeight(32);	
	CollisionRect = { 0, 0, 32, 32 };
	VelocityY = 12;
	VelocityX = InVelocityX;

	PlayAnimation(GResourceManager->FireAnimation);
}

FireBallSprite::~FireBallSprite()
{
	ThePlayer->OnFireBallDestroy();
	
	Sprite* Explosion = new Sprite(GResourceManager->ExplosionTexture->Texture);

	Explosion->SetPosition(PosX, PosY);
	Explosion->SetWidth(64);
	Explosion->SetHeight(64);
	Explosion->PlayAnimation(GResourceManager->ExplosionAnimation, false);
	SimpleSprites.push_back(Explosion);
}

void FireBallSprite::Tick(double DeltaTime)
{
	Sprite::Tick(DeltaTime);
	
	int NumPixelsToMove = abs(VelocityX);
	SDL_Rect NewRect = GetScreenSpaceCustomRect();
	
	if (VelocityY < 0)
	{
		VelocityY += BASE_FALL_VELOCITY / 2;

		if (VelocityY > 0)
		{
			VelocityY = 10;
		}
	}

	vector<TileInfo> HitTileLocs;

	for (int x = 0; x < NumPixelsToMove; x++)
	{
		NewRect.x += VelocityX > 0 ? 1 : -1;

		HitTileLocs.clear();
		if (TheMap->CheckCollision(NewRect, HitTileLocs, true))
		{	
			bool bDestroyedBrick = false;
			bPendingDelete = true;

			for (int i = 0; i < HitTileLocs.size(); i++)
			{								
				if (TheMap->IsDestroyableByFireTile(HitTileLocs[i].MetaTileType))
				{
					TheMap->DoBrickBreak(HitTileLocs[i].Location.x, HitTileLocs[i].Location.y);
					bPendingDelete = true;
					bDestroyedBrick = true;
					return;
				}
			}

			if (!bDestroyedBrick)
			{
				Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
			}
			break;
		}
		else
		{
			PosX += VelocityX > 0 ? 1 : -1;
			CheckCollisionWithSprites();
		}
	}
	
	NumPixelsToMove = abs(VelocityY);
	NewRect = GetScreenSpaceCustomRect();

	for (int y = 0; y < NumPixelsToMove; y++)
	{
		NewRect.y += VelocityY > 0 ? 1 : -1;

		HitTileLocs.clear();
		if (TheMap->CheckCollision(NewRect, HitTileLocs, true))
		{
			for (int i = 0; i < HitTileLocs.size(); i++)
			{
				if (TheMap->IsDestroyableByFireTile(HitTileLocs[i].MetaTileType))
				{
					TheMap->DoBrickBreak(HitTileLocs[i].Location.x, HitTileLocs[i].Location.y);
					bPendingDelete = true;
					return;
				}
			}
		}

		if (IsOnGround() && VelocityY >= 0)
		{
			NewRect = GetScreenSpaceCustomRect();			
			VelocityY = -10;
			NewRect.y++;
			HitTileLocs.clear();
			if (TheMap->CheckCollision(NewRect, HitTileLocs, true))
			{
				for (int i = 0; i < HitTileLocs.size(); i++)
				{
					if (TheMap->IsDestroyableByFireTile(HitTileLocs[i].MetaTileType))
					{
						TheMap->DoBrickBreak(HitTileLocs[i].Location.x, HitTileLocs[i].Location.y);
						bPendingDelete = true;
						return;
					}
				}
			}
			break;
		}
		else
		{
			PosY += VelocityY > 0 ? 1 : -1;
			HitTileLocs.clear();
			if (TheMap->CheckCollision(NewRect, HitTileLocs, true))
			{
				for (int i = 0; i < HitTileLocs.size(); i++)
				{
					if (TheMap->IsDestroyableByFireTile(HitTileLocs[i].MetaTileType))
					{
						TheMap->DoBrickBreak(HitTileLocs[i].Location.x, HitTileLocs[i].Location.y);
						bPendingDelete = true;
						return;
					}
				}
			}
			CheckCollisionWithSprites();
		}
	}
	
	Rect.x = PosX;
	Rect.y = PosY;
	
}

bool FireBallSprite::Interact(EnemySprite* Enemy)
{
	if (Enemy->IsFireable())
	{
		bPendingDelete = true;
		// TODO: Get enemy point value???
		ThePlayer->AddScore(100, Enemy->GetPosX(), Enemy->GetPosY());
		Enemy->GetFired();
	}
	else if (!Enemy->IsDying())
	{
		Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
		bPendingDelete = true;
	}

	return false;
}

PointSprite::PointSprite(int X, int Y, string Value)
{
	PointString = Value;
	PosX = X - TheMap->GetScrollX();
	PosY = Y - TheMap->GetScrollY();
	FrameCountDown = 40;

	VelocityY = -4;
}

PointSprite::PointSprite(int X, int Y, int Value)
{
	char PointStringTemp[10];
	itoa(Value, PointStringTemp, 10);

	PointString = PointStringTemp;
	PosX = X - TheMap->GetScrollX();
	PosY = Y - TheMap->GetScrollY();
	FrameCountDown = 40;

	VelocityY = -4;
}

void PointSprite::Tick(double DeltaTime)
{
	PosY += VelocityY;

	FrameCountDown--;

	if (FrameCountDown <= 0)
	{
		bPendingDelete = true;
	}
}

void PointSprite::Render(SDL_Renderer* Renderer, int ResourceNum)
{	
	DrawBitmapText(PointString, PosX, PosY, 24, 24, GRenderer, FontShadowedWhite, 0.75, 1, false);
}

FlagPoleSprite::FlagPoleSprite(int X, int Y, bool bSecretExit)
	: Sprite(GResourceManager->FlagPoleTexture->Texture)
{
	if (bSecretExit)
	{
		FlagTexture = GResourceManager->SecretFlagTexture->Texture;
		SetTexture(GResourceManager->SecretFlagPoleTexture->Texture);
	}
	else
	{
		FlagTexture = GResourceManager->FlagTexture->Texture;
	}
	bFlagTraveling = false;
	FlagTravelCountDown = 63;
	bDeleteWhenNotVisible = false;
	bIsSecretExit = bSecretExit;
	SetPosition(X, Y - 608);
	FlagY = 30;
	CollisionRect = { 4, 24, 56, 584 };
	SetWidth(64);
	SetHeight(608);	
}

void FlagPoleSprite::Tick(double DeltaTime)
{
	if (bFlagTraveling && FlagTravelCountDown > 0)
	{
		if (FlagTravelCountDown > 1)
		{
			FlagY += 8;
		}

		FlagTravelCountDown--;

		if (FlagTravelCountDown == 0)
		{
			ThePlayer->OnFlagFinished();
		}
	}

	Sprite::Tick(DeltaTime);
}

void FlagPoleSprite::Interact(PlayerSprite* Player)
{
	if (!bFlagTraveling)
	{
		double PlayerTranslatedY = ThePlayer->GetScreenSpaceCollisionRect().y + ThePlayer->GetScreenSpaceCollisionRect().h;
		PlayerTranslatedY -= (Rect.y + CollisionRect.y) - TheMap->GetScrollY();
		float GrabPercent = 1 - PlayerTranslatedY / Rect.h;

		GrabPercent *= 100;

		GrabPercent = 8 * (GrabPercent / 100);

		Points = StompPoints[(int)round(GrabPercent)];
		bFlagTraveling = true;
		Player->GrabFlagPole(this);
	}
}

void FlagPoleSprite::Render(SDL_Renderer* Renderer, int ResourceNum)
{
	if (TheMap->InMapWindow(PosX, PosY))
	{
		Sprite::Render(Renderer);
		SDL_Rect DstRect = { PosX - 32 - TheMap->GetScrollX(), PosY - TheMap->GetScrollY() + FlagY, 64, 64 };
		SDL_RenderCopy(Renderer, FlagTexture, NULL, &DstRect);

		if (bFlagTraveling)
		{
			char TempChar[10];
			itoa(Points, TempChar, 10);
			DrawBitmapText(TempChar, (PosX + 48) - TheMap->GetScrollX(), Rect.h - (FlagY - TheMap->GetScrollY()) - 128, 24, 24, Renderer, FontShadowedWhite, 0.75, 1, false);
		}
	}
}

FlagPoleSprite::~FlagPoleSprite()
{

}

bool FlagPoleSprite::IsDone()
{
	return FlagTravelCountDown == 0;
}

PlayerFlagSprite::PlayerFlagSprite(int X, int Y)
{
	RenderLayer = RENDER_LAYER_BEHIND_BG;
	Sprite(GResourceManager->PlayerFlagTexture->Texture);
	SetTexture(GResourceManager->PlayerFlagTexture->Texture);
	SetPosition(X, Y);
	SetWidth(64);
	SetHeight(64);
	DestY = Y - 128;
}

void PlayerFlagSprite::Tick(double DeltaTime)
{
	//Sprite::Tick(DeltaTime);

	if (PosY > DestY)
	{
		PosY -= 5;

		if (PosY <= DestY)
		{
			PosY = DestY;
			TheGame->OnPlayerFlagDone();
		}
	}

	Rect.y = PosY;
}


EventSprite::EventSprite(string InCaption) :
	Sprite(GResourceManager->EventBoxTexture->Texture)
{
	PosX = -100;	
	CountDown = 300;
	Caption = InCaption;
	RenderLayer = RENDER_LAYER_ABOVE_ALL;
	SDL_Surface* FontSurface = TTF_RenderText_Solid(MainFont, InCaption.c_str(), { 200, 200, 200, 200 });
	CaptionTexture = SDL_CreateTextureFromSurface(GRenderer, FontSurface);

	Uint32 Format;
	int Access, CaptionWidth, CaptionHeight;
	SDL_QueryTexture(CaptionTexture, &Format, &Access, &CaptionWidth, &CaptionHeight);
	CaptionRect = { 0, 0, CaptionWidth, CaptionHeight };
		
	SDL_FreeSurface(FontSurface);
	bDrawScreenSpace = true;
	Mix_PlayChannel(CHANNEL_EVENT, EventSound, 0);
	BlendColor = { 255, 255, 255, 255 };
	SDL_SetTextureBlendMode(Texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(CaptionTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(Texture, BlendColor.a);

	Rect.x = PosX;
	Rect.y = PosY;
}

void EventSprite::Render(SDL_Renderer* Renderer, int ResourceNum)
{
	Sprite::Render(Renderer);
	SDL_Rect CaptionDstRect = { PosX + Rect.w / 2 - CaptionRect.w / 2, PosY + 8, CaptionRect.w, CaptionRect.h };
	SDL_RenderCopy(Renderer, CaptionTexture, &CaptionRect, &CaptionDstRect);
	
}

void EventSprite::Tick(double DeltaTime)
{
	CountDown--;

	if (PosX < 0)
	{
		PosX += 6;

		if (PosX > 0)
		{
			PosX = 0;
		}
	}

	if (CountDown <= 51)
	{
		SDL_SetTextureAlphaMod(Texture, BlendColor.a);
		SDL_SetTextureAlphaMod(CaptionTexture, BlendColor.a);
		BlendColor.a -= 5;
	}

	if (CountDown == 0)
	{
		Delete();
	}

	Rect.x = PosX;
	Rect.y = PosY;
}

EventSprite::~EventSprite()
{
	SDL_DestroyTexture(CaptionTexture);
}

SpriteSpawnPoint::SpriteSpawnPoint(int X, int Y, eSpriteType SpriteToSpawn, bool bInUseSpawnAnimation, int InFrameDelay) :
	Sprite(GResourceManager->SpriteSpawnTexture->Texture)
{	
	bDeleteAfterAnimation = false;
	bDeleteWhenNotVisible = false;
	bInRange = false;
	bUseSpawnAnimation = bInUseSpawnAnimation;
	SpriteTypeToSpawn = SpriteToSpawn;
	
	SetWidth(64);
	SetHeight(64);
	SetPosition(X, Y);

	FrameDelay = InFrameDelay;
	CountDown = InFrameDelay;
}

void SpriteSpawnPoint::Render(SDL_Renderer* Renderer, int ResourceNum)
{
	if (bRenderCollision || AnimData.Anim)
	{
		Sprite::Render(Renderer, ResourceNum);
	}
}

void SpriteSpawnPoint::Tick(double DeltaTime)
{
	if (bPendingDelete)
	{
		return;
	}

	Sprite::Tick(DeltaTime);

	if (bWaitingForAnimation)
	{		
		if (AnimData.bFinished)
		{
			SpawnSprite();
			bWaitingForAnimation = false;
			PlayAnimation(nullptr);
			
		}
		else
		{
			return;
		}
	}

	if (InMapWindow({ SPAWN_DISTANCE, SPAWN_DISTANCE }))
	{		
		if (CountDown <= 0)  // Have to be in range for as long as FrameDelay before spawning
		{
			if (!bInRange)
			{
				if (bUseSpawnAnimation)
				{
					bWaitingForAnimation = true;
					PlayAnimation(GResourceManager->SpriteSpawnAnimation);
					AnimData.bLoop = false;
				}
				else
				{
					SpawnSprite();
				}

			}
			bInRange = true;
		}
		else
		{
			CountDown--;
		}
	}
	else
	{
		CountDown = FrameDelay;
		bInRange = false;
	}
	
}

void SpriteSpawnPoint::SpawnSprite()
{
	

	switch (SpriteTypeToSpawn)
	{
	case ST_OLD_MAN:		
		SimpleSprites.push_back(new OldManSprite(Rect.x, Rect.y));
		break;

	case ST_FIRE:		
		EnemySprites.push_back(new FireEnemySprite(this));
		break;

	case ST_SWORD:		
		ItemSprites.push_back(new AdventureSwordItemSprite(Rect.x, Rect.y));
		break;

	}

	CountDown = FrameDelay;
}

OldManSprite::OldManSprite(int X, int Y) :
	Sprite(GResourceManager->OldManTexture->Texture)
{
	CountDown = 0;
	SetWidth(64);
	SetHeight(64);
	SetPosition(X, Y);
	TheGame->DoTextBox(SCREEN_WIDTH / 2 - 350, 358, 700, 430, "IT'S DANGEROUS TO GO \\n  \\S \\S ALONE! TAKE THIS.", false, TBS_Zelda);
}

void OldManSprite::Tick(double DeltaTime)
{
	if (CountDown > 0)
	{
		bVisible = (int)CountDown % 2;

		CountDown--;

		if (CountDown == 0)
		{
			Delete();
		}
	}
}

void OldManSprite::FadeOut()
{
	CountDown = 63;
}

AdventureFireSprite::AdventureFireSprite(int X, int Y) :
	Sprite(nullptr)
{	
	PlayAnimation(GResourceManager->AdventureFireAnimation);
	SetWidth(64);
	SetHeight(64);
	SetPosition(X, Y);
}

AdventureSwordAttack::AdventureSwordAttack(int X, int Y, bool bShoot)
	: Sprite(GResourceManager->AdventureSwordTexture->Texture)
{
	SetWidth(64);
	SetHeight(64);
	SetPosition(X, Y);
	CollisionRect = { 27, 0, 12, 46 };
}

void AdventureSwordAttack::SetDirection(eDirection NewDirection)
{
	CurrentDirection = NewDirection;
	SetFlip(SDL_FLIP_NONE);

	if (CurrentDirection == DIRECTION_UP)
	{
		CollisionRect = { 27, 0, 12, 46 };
		RenderAngle = 0;
	}
	else if (CurrentDirection == DIRECTION_DOWN)
	{
		SetFlip(SDL_FLIP_VERTICAL);
		CollisionRect = { 27, 18, 12, 46 };
		RenderAngle = 0;
	}
	else if (CurrentDirection == DIRECTION_LEFT)
	{
		CollisionRect = {  0, 25, 46, 12 };
		RenderAngle = 270;
	}
	else if (CurrentDirection == DIRECTION_RIGHT)
	{
		CollisionRect = { 18, 27, 46, 12 };
		RenderAngle = 90;
	}
}

SDL_Rect AdventureSwordAttack::GetScreenSpaceCollisionRect()
{
	return Sprite::GetScreenSpaceCollisionRect();
}

void AdventureSwordAttack::Tick(double DeltaTime)
{
	Sprite::Tick(DeltaTime);
	CheckCollisionWithSprites();
}

bool AdventureSwordAttack::Interact(class EnemySprite* Enemy)
{
	Enemy->GetFired();

	return false;
}