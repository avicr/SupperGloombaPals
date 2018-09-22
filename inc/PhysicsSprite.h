#pragma once

#include "../inc/Globals.h"
#include "Sprite.h"

class PhysicsSprite : public Sprite
{
protected:	
	bool bIsInAir;

	PhysicsSprite()
	{ 
		bIsInAir = false; 
	}
public:
	virtual void Tick(double DeltaTime);	
};
