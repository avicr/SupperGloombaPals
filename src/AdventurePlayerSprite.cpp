#include "../inc/AdventurePlayerSprite.h"

AdventurePlayerSprite::AdventurePlayerSprite()
{
	HP = 6;
	bIsAdventure = true;
	SwordAnimationCount = 129;
	bSpriteVisible = true;
}

void AdventurePlayerSprite::TakeDamage()
{
	HP--;

	if (HP <= 0)
	{
		// TODO: Player adventure death animation
		BeginDie();
	}
	else
	{
		InvincibleCount = NUM_INVINCIBLE_FRAMES;
	}
}