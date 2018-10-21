#pragma once
#include "Globals.h"
#include "PlayerSprite.h"
#include "PhysicsSprite.h"

enum eItemType
{
	ITEM_MUSHROOM = 0,
	ITEM_FLOWER,
	ITEM_STAR,
	ITEM_ONE_UP,
	ITEM_MAGIC_MUSHROOM,
	ITEM_PLOT_DEVICE,
	ITEM_NONE = 255
};


class ItemSprite : public PhysicsSprite
{
protected:
	eItemType ItemType;
	bool bGrowing;
	int DestintationY; // Y location we are growing to
public:
	ItemSprite(int X, int Y, bool bDoGrowAnimation = true);
	virtual SDL_Rect GetScreenSpaceCustomRect();
	virtual eItemType GetItem() { return ItemType; }
	virtual void GetCollected();
	virtual void GetBricked(int TileX, int TileY);
	virtual void GetFired() {};
};

class BigMushroomItemSprite : public ItemSprite
{
public:
	BigMushroomItemSprite(int X, int Y, bool bDoGrowAnimation = true);
	virtual void Tick(double DeltaTime);	
};

class OneUpMushroomSprite : public BigMushroomItemSprite
{
public:
	OneUpMushroomSprite(int X, int Y, bool bDoGrowAnimation = true);
};

class FlowerItemSprite : public ItemSprite
{
public: 
	FlowerItemSprite(int X, int Y, bool bGrowAnmation = true);
	virtual void Tick(double DeltaTime);
};

class StarItemSprite : public ItemSprite
{
public:
	StarItemSprite(int X, int Y, bool bDoGrowAnimation = true);
	virtual void Tick(double DeltaTime);	

};

class MagicMushroomItemSprite : public BigMushroomItemSprite
{
public:
	MagicMushroomItemSprite(int X, int Y, bool bDoGrowAnimation = true);
	virtual void Tick(double DeltaTime);
};

class PlotDeviceItemSprite : public FlowerItemSprite
{
public:
	PlotDeviceItemSprite(int X, int Y, bool bGrowAnmation = true);
};