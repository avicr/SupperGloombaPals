#include "../inc/EmptyBlockSprite.h"
#include "../inc/TMXMap.h"
#include "../inc/EnemySprite.h"
#include "../inc/SpriteList.h"
#include "../inc/ItemSprite.h"

EmptyBlockSprite::EmptyBlockSprite(int X, int Y, AnimationResource* Anim, bool bReplaceTileWhenFinished, int ReplacementTile, int InTileX, int InTileY, eItemType InItemToSpawn)
{
	ItemToSpawn = InItemToSpawn;
	AliveFrameCount = 0;
	TileID = ReplacementTile;
	bReplace = bReplaceTileWhenFinished;
	SetPosition(X, Y);
	SetWidth(64);
	SetHeight(128);
	CollisionRect = { 0, 0, Rect.w, Rect.h };
	TileX = InTileX;
	TileY = InTileY;
	PlayAnimation(Anim, false);
}

void EmptyBlockSprite::TickAnimation(double DeltaTime)
{
	if (AnimData.Anim)
	{
		AnimData.CountDown -= DeltaTime * AnimData.PlayRate;

		if (AnimData.CountDown <= 0)
		{
			AnimData.CurrentFrameIndex++;
			if (AnimData.CurrentFrameIndex >= AnimData.Anim->GetFrameCount())
			{
				if (bReplace)
				{
					TheMap->SetForegroundTile(TileX, TileY, TileID);
				}

				if (ItemToSpawn != ITEM_NONE)
				{
					if (ItemToSpawn == ITEM_MUSHROOM)
					{
						ItemSprites.push_back(new BigMushroomItemSprite(TileX * 64, (TileY - 1) * 64));
					}

					if (ItemToSpawn == ITEM_ONE_UP)
					{
						ItemSprites.push_back(new OneUpMushroomSprite(TileX * 64, (TileY - 1) * 64));
					}
					else if (ItemToSpawn == ITEM_FLOWER)
					{
						ItemSprites.push_back(new FlowerItemSprite(TileX * 64, (TileY - 1) * 64));
					}
					else if (ItemToSpawn == ITEM_STAR)
					{
						ItemSprites.push_back(new StarItemSprite(TileX * 64, (TileY - 1) * 64));
					}
					else if (ItemToSpawn == ITEM_MAGIC_MUSHROOM)
					{
						ItemSprites.push_back(new MagicMushroomItemSprite(TileX * 64, (TileY - 1) * 64));
					}
					else if (ItemToSpawn == ITEM_PLOT_DEVICE)
					{
						ItemSprites.push_back(new PlotDeviceItemSprite(TileX * 64, (TileY - 1) * 64));
					}

				}
				bPendingDelete = true;
			}
			UpdateAnimationData();
		}
	}

	// Ugh
	AliveFrameCount++;

	if (AliveFrameCount > 16)
	{
		bPendingDelete = true; // fail safe for bricks that don't animate
	}
}

bool EmptyBlockSprite::Interact(EnemySprite* Other)
{
	if (AliveFrameCount < 4 && !Other->IsDying())
	{
		Other->GetBricked(PosX / 64 * 64, PosY / 64 * 64);
		ThePlayer->AddScore(100, Other->GetPosX(), Other->GetPosY() - 64);
	}

	return true;	
}

void EmptyBlockSprite::Interact(ItemSprite* Other)
{
	if (AliveFrameCount < 4)
	{
		Other->GetBricked(PosX / 64 * 64, PosY / 64 * 64);
	}

}

void EmptyBlockSprite::Interact(PlayerSprite* Player)
{
	if (AliveFrameCount < 4)
	{
		Player->Launch();
	}
}
