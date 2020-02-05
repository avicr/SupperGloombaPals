#pragma once
#include "PlayerSprite.h"

class AdventurePlayerSprite : public PlayerSprite
{
public:

	AdventurePlayerSprite();
	void TakeDamage();

protected:
	int HP;
};