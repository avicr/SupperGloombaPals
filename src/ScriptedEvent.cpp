#include "../inc/ScriptedEvent.h"
#include "../inc/Sprite.h"
#include "../inc/SimpleSprites.h"
#include "../inc/EnemySprite.h"
#include "../inc/SpriteList.h"

AdventureLockedDoorSprite::AdventureLockedDoorSprite() :
	Sprite(NULL)
{
	bDeleteAfterAnimation = false;
	PlayAnimation(GResourceManager->AdventureDoorCloseAnimation, false);
	SetPosition(3712, 2560);
	SetWidth(128);
	SetHeight(128);
}

ScriptedEvent::ScriptedEvent()
{
	Counter = 0;
}

OktoSpawnEvent::OktoSpawnEvent()
{
	DoorSprite = new AdventureLockedDoorSprite();
	SimpleSprites.push_back(DoorSprite);

	ThePlayer->SetFrozen(true);
}

void OktoSpawnEvent::Tick()
{

	if (Counter == 28)
	{
		
		GiantOktoEnemy = new GiantOkto(3718, 2130);
		//GiantOktoEnemy = new GiantOkto(ThePlayer->GetPosX(), ThePlayer->GetPosY());
				

		EnemySprites.push_back(GiantOktoEnemy);
		ThePlayer->SetFrozen(false);
	}
	Counter++;
}