#pragma once

#include "Globals.h"
#include "Sprite.h"
#include "ItemSprite.h"

class EmptyBlockSprite : public Sprite
{
protected:
	eItemType ItemToSpawn;
	int AliveFrameCount;
	bool bReplace;
	int TileID;
	int TileX;
	int TileY;
public:
	EmptyBlockSprite(int X, int Y, AnimationResource* Anim, bool bReplaceTileWhenFinished = false, int ReplacementTile = -1, int InTileX = 0, int InTileY = 0, eItemType InItemToSpawn = ITEM_NONE);
	
	void TickAnimation(double DeltaTime);
	virtual bool Interact(EnemySprite* Other);	
	virtual void Interact(ItemSprite* Other);	
	virtual void Interact(PlayerSprite* Player);
};