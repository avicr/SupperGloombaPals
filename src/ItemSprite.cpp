#include "../inc/ItemSprite.h"
#include "../inc/TMXMap.h"
#include "../inc/SpriteList.h"
#include "../inc/ScriptedEvent.h"

ItemSprite::ItemSprite(int X, int Y, bool bDoGrowAnimation)
{	
	ItemType = ITEM_NONE;	
	RenderLayer = RENDER_LAYER_TOP;
	bGrowing = bDoGrowAnimation;

	// If we are growing, set our Y to one tile lower than where we want to be
	if (bGrowing)
	{
		RenderLayer = RENDER_LAYER_BEHIND_FG;
		DestintationY = Y;
		Y += 64;
		VelocityY = -2;
		Mix_PlayChannel(CHANNEL_POWER_UP, PowerUpSound, 0);
	}

	SetPosition(X, Y);
	
}

SDL_Rect ItemSprite::GetScreenSpaceCustomRect()
{
	return{ (int)PosX - (int)TheMap->GetScrollX(), (int)PosY - (int)TheMap->GetScrollY(), Rect.w, Rect.h -1 };
}

void ItemSprite::GetCollected()
{
	bPendingDelete = true;
}

void ItemSprite::GetBricked(int TileX, int TileY)
{
	// Only get bricked if we're not in the air
	if (VelocityY == 0)
	{
		SDL_Rect MapSpaceCollisionRect = GetMapSpaceCollisionRect();
		if ((MapSpaceCollisionRect.x) / 64 * 64 < TileX)
		{
			VelocityX *= -1;
		}
		PosY--;
		Rect.y--;
		VelocityY = -12;
	}
}

BigMushroomItemSprite::BigMushroomItemSprite(int X, int Y, bool bDoGrowAnimation)
	: ItemSprite(X, Y, bDoGrowAnimation)
{
	Sprite::Sprite(GResourceManager->BigMushroomTexture->Texture);
	SetTexture(GResourceManager->BigMushroomTexture->Texture);

	ItemType = ITEM_MUSHROOM;

	CollisionRect = { 8, 0, 50, 50 };
	VelocityX = 3.1;
	
}

OneUpMushroomSprite::OneUpMushroomSprite(int X, int Y, bool bDoGrowAnimation)
	: BigMushroomItemSprite(X, Y, bDoGrowAnimation)
{
	Sprite::Sprite(GResourceManager->OneUpMushroomTexture->Texture);
	SetTexture(GResourceManager->OneUpMushroomTexture->Texture);

	ItemType = ITEM_ONE_UP;

	CollisionRect = { 8, 0, 50, 50 };
	VelocityX = 3.1;

}

void BigMushroomItemSprite::Tick(double DeltaTime)
{	
	if (!bGrowing)
	{
		PhysicsSprite::Tick(DeltaTime);

		int NumPixelsToMove = round(fabs(VelocityX));
		SDL_Rect NewRect = GetScreenSpaceCustomRect();

		for (int x = 0; x < NumPixelsToMove; x++)
		{
			NewRect.x += VelocityX > 0 ? 1 : -1;

			if (TheMap->CheckCollision(NewRect, true))
			{
				// Cap max speed when colliding with a wall
				VelocityX *= -1;

				SetFlip(VelocityX < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
				break;
			}
			else
			{
				PosX += VelocityX > 0 ? 1 : -1;
				CheckCollisionWithSprites();
			}
		}

		NumPixelsToMove = 0;
		if (!IsOnGround())
		{
			VelocityY += BASE_FALL_VELOCITY;

			NumPixelsToMove = round(fabs(VelocityY));
			NewRect = GetScreenSpaceCustomRect();			
		}	
		else
		{			
			VelocityY = 0;
		}

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
		PosY += VelocityY;
		if (PosY <= DestintationY)
		{
			PosY = DestintationY;
			bGrowing = false;
			RenderLayer = RENDER_LAYER_TOP;
			VelocityY = 0;
		}
	}

	Rect.x = PosX;// -TheMap->GetScrollX();
	Rect.y = PosY;
}

FlowerItemSprite::FlowerItemSprite(int X, int Y, bool bDoGrowAnimation)
	: ItemSprite(X, Y, bDoGrowAnimation)
{
	Sprite::Sprite(GResourceManager->FlowerTexture->Texture);
	PlayAnimation(GResourceManager->FlowerAnimation);
	SetWidth(64);
	SetHeight(64);
	ItemType = ITEM_FLOWER;

	CollisionRect = { 8, 0, 50, 50 };	

}

void FlowerItemSprite::Tick(double DeltaTime)
{			
	Sprite::Tick(DeltaTime);

	if (bGrowing)
	{
		PosY += VelocityY;
		if (PosY <= DestintationY)
		{
			PosY = DestintationY;
			bGrowing = false;
			RenderLayer = RENDER_LAYER_TOP;
			VelocityY = 0;
		}
		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;
	}	
}

StarItemSprite::StarItemSprite(int X, int Y, bool bDoGrowAnimation)
	: ItemSprite(X, Y, bDoGrowAnimation)
{
	Sprite::Sprite(GResourceManager->StarTexture->Texture);
	PlayAnimation(GResourceManager->StarAnimation);
	SetWidth(64);
	SetHeight(64);
	ItemType = ITEM_STAR;

	CollisionRect = { 8, 8, 50, 42 };

}

void StarItemSprite::Tick(double DeltaTime)
{
	Sprite::Tick(DeltaTime);

	if (bGrowing)
	{
		PosY += VelocityY;
		if (PosY <= DestintationY)
		{
			PosY = DestintationY;
			bGrowing = false;
			RenderLayer = RENDER_LAYER_TOP;
			VelocityY = -10;
			VelocityX = 3;
		}				
	}
	else
	{		
		int NumPixelsToMove = abs(VelocityX);
		SDL_Rect NewRect = GetScreenSpaceCustomRect();
		NewRect.y + 1;
		bool bChangedVerticalDirection = false;


		if (VelocityY < 0)
		{			
			VelocityY += 0.35;

			// Did we change directions?
			if (VelocityY < 0 && TheMap->CheckCollision(NewRect, true))
			{
				bChangedVerticalDirection = true;
				PosY += 1;
				Rect.y = PosY;
				VelocityY = 10;
			}
			else if (VelocityY > 0)
			{				
				VelocityY = 7;
			}
		}
		else if (VelocityY > 0)
		{			
			VelocityY += 0.35;
		}		
		
		NewRect = GetScreenSpaceCustomRect();
		
		for (int x = 0; x < NumPixelsToMove; x++)
		{
			NewRect.x += VelocityX > 0 ? 1 : -1;

			if (TheMap->CheckCollision(NewRect, true) && !bChangedVerticalDirection)
			{
				VelocityX *= -1;
				break;
			}
			else
			{
				PosX += VelocityX > 0 ? 1 : -1;
				CheckCollisionWithSprites();
			}
		}
		

		NumPixelsToMove = abs(VelocityY);

		for (int y = 0; y < NumPixelsToMove; y++)
		{

			if (IsOnGround() && VelocityY >= 0)
			{
				VelocityY = -10;
				break;
			}
			else
			{
				PosY += VelocityY > 0 ? 1 : -1;
				CheckCollisionWithSprites();
			}
		}
	}

	Rect.x = PosX;// -TheMap->GetScrollX();
	Rect.y = PosY;
}

MagicMushroomItemSprite::MagicMushroomItemSprite(int X, int Y, bool bDoGrowAnimation)
	: BigMushroomItemSprite(X, Y, bDoGrowAnimation)
{
	Sprite::Sprite(GResourceManager->MagicMushroomTexture->Texture);
	SetTexture(GResourceManager->MagicMushroomTexture->Texture);

	ItemType = ITEM_MAGIC_MUSHROOM;

	CollisionRect = { 8, 0, 50, 50 };
	VelocityX = 4.5;
	VelocityY = -5;
}

void MagicMushroomItemSprite::Tick(double DeltaTime)
{
	if (!bGrowing && IsOnGround())
	{
		VelocityY = 0;
	}

	if (rand() % 200 < 2)
	{
		VelocityX *= -1;
	}
	if (rand() % 100 < 2)
	{
		VelocityX += 0.25;
	}
	else if (VelocityY == 0 && (rand() % 100) < 2)
	{
		VelocityY = -18;
		Mix_PlayChannel(CHANNEL_JUMP, JumpSound, 0);
	}

	// Old big mushroom routine, don't fuck with this
	if (!bGrowing)
	{
		PhysicsSprite::Tick(DeltaTime);

		int NumPixelsToMove = round(fabs(VelocityX));
		SDL_Rect NewRect = GetScreenSpaceCustomRect();

		for (int x = 0; x < NumPixelsToMove; x++)
		{
			NewRect.x += VelocityX > 0 ? 1 : -1;

			if (TheMap->CheckCollision(NewRect, true))
			{
				// Cap max speed when colliding with a wall
				VelocityX *= -1;

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
		}
		
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
		PosY += VelocityY;
		if (PosY <= DestintationY)
		{
			PosY = DestintationY;
			bGrowing = false;
			RenderLayer = RENDER_LAYER_TOP;
			VelocityY = 0;
		}
	}

	Rect.x = PosX;// -TheMap->GetScrollX();
	Rect.y = PosY;
}

PlotDeviceItemSprite::PlotDeviceItemSprite(int X, int Y, bool bDoGrowAnimation)
	: FlowerItemSprite(X, Y, bDoGrowAnimation)
{
	SetTexture(GResourceManager->PlotDeviceTexture->Texture);	
	AnimData.Anim = NULL;
	SetWidth(64);
	SetHeight(64);
	ItemType = ITEM_PLOT_DEVICE;

	CollisionRect = { 8, 0, 50, 50 };

}

AdventureSwordItemSprite::AdventureSwordItemSprite(int X, int Y) :
	ItemSprite(X, Y, false)
{
	SetTexture(GResourceManager->AdventureSwordTexture->Texture);	
	ItemType = ITEM_ADVENTURE_SWORD;

	CollisionRect = { 26, 40, 18, 24 };
}

void AdventureSwordItemSprite::GetCollected()
{
	// FIXME HACK
	// Hardcode the entrance to be solid so the player can't get out	
	TheMap->SetMetaLayerTile(58, 40, eTileMetaType::TILE_COLLISION);
	TheMap->SetMetaLayerTile(59, 40, eTileMetaType::TILE_COLLISION);

	// Make the row where the fire is passable now
	for (int i = 0; i < 12; i++)
	{
		TheMap->SetMetaLayerTile(53 + i, 35, eTileMetaType::TILE_NONE);
	}
}

TriforceItemSprite::TriforceItemSprite(int X, int Y)
	: ItemSprite(X, Y, false)
{
	SetTexture(GResourceManager->AdventureTriangleTexture->Texture);
	AnimData.Anim = NULL;
	SetWidth(64);
	SetHeight(64);
	ItemType = ITEM_ADVENTURE_TRIANGLE;

	CollisionRect = { 8, 0, 50, 50 };
}

void TriforceItemSprite::GetCollected()
{

	ItemSprite::GetCollected();
}