#pragma once
#include "Globals.h"
#include "Sprite.h"

class ScriptedEvent
{
protected:
	int Counter;
public:
	ScriptedEvent();
	virtual void Tick() {};
};

class AdventureLockedDoorSprite : public Sprite
{
public:
	AdventureLockedDoorSprite();
};

class OktoSpawnEvent : public ScriptedEvent
{
protected:
	Sprite* DoorSprite;
	class GiantOkto* GiantOktoEnemy;

public:
	OktoSpawnEvent();
	virtual void Tick();
};

