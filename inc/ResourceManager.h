#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#ifdef _WIN32
#include <SDL.h>
#else
#include "SDL2/SDL.h"
#endif
#include <string>
#include "Globals.h"
#include "AssetResource.h"
#include "Animation.h"

using namespace std;

#define DECLARE_TEXTURE_RESOURCE(RESOURCE_NAME, FILE_NAME, CREATE_ON_SECOND_RENDERER) \
class TEXTURE_RESOURCE_##RESOURCE_NAME : public TextureResource \
{ \
	public: \
	TEXTURE_RESOURCE_##RESOURCE_NAME() : TextureResource(FILE_NAME, CREATE_ON_SECOND_RENDERER) {ResourceManager::RESOURCE_NAME = new TEXTURE_RESOURCE_##RESOURCE_NAME(*this);} \
	~TEXTURE_RESOURCE_##RESOURCE_NAME() {}; \
}; \
public : \
static TEXTURE_RESOURCE_##RESOURCE_NAME *RESOURCE_NAME;  \
private : \
static TEXTURE_RESOURCE_##RESOURCE_NAME _##RESOURCE_NAME;  

#define INIT_TEXTURE_RESOURCE(RESOURCE_NAME) \
ResourceManager::TEXTURE_RESOURCE_##RESOURCE_NAME ResourceManager::_##RESOURCE_NAME; \
ResourceManager::TEXTURE_RESOURCE_##RESOURCE_NAME *ResourceManager::RESOURCE_NAME = NULL; 

class ResourceManager
{
	friend AssetResource;

public:
	
	DECLARE_TEXTURE_RESOURCE(PlayerGoombaTexture, "goomba.bmp", true)
	DECLARE_TEXTURE_RESOURCE(PlayerGoombaTallTexture, "PlayerGoombaTall.bmp", true)
	DECLARE_TEXTURE_RESOURCE(GoombaGrowTexture, "playergrow.bmp", true)
	DECLARE_TEXTURE_RESOURCE(GoombaDuckTexture, "goombaduck.bmp", true)
	DECLARE_TEXTURE_RESOURCE(PlayerGoombaDeadTexture, "goombadead.bmp", true)
	DECLARE_TEXTURE_RESOURCE(EventBoxTexture, "eventbox.bmp", true)

	DECLARE_TEXTURE_RESOURCE(AdventureHUDTexture, "AdventureHUD.bmp", false);
	DECLARE_TEXTURE_RESOURCE(AdventureAttackDownTexture, "AdventureAttackDown.bmp", false);
	DECLARE_TEXTURE_RESOURCE(AdventureAttackUpTexture, "AdventureAttackUp.bmp", false);
	DECLARE_TEXTURE_RESOURCE(AdventureAttackSideTexture, "AdventureAttackSide.bmp", false);
	DECLARE_TEXTURE_RESOURCE(AdventureDownTexture, "AdventureDown.bmp", false)
	DECLARE_TEXTURE_RESOURCE(AdventureUpTexture, "AdventureUp.bmp", false)
	DECLARE_TEXTURE_RESOURCE(AdventureSideTexture, "AdventureSide.bmp", false)
	DECLARE_TEXTURE_RESOURCE(AdventureGetTexture, "AdventureGet.bmp", false)

	DECLARE_TEXTURE_RESOURCE(AdventureDoorCloseTexture, "AdventureDoorClose.bmp", false)
	DECLARE_TEXTURE_RESOURCE(OktoTexture, "okto.bmp", false)
	DECLARE_TEXTURE_RESOURCE(OktoSideTexture, "oktoside.bmp", false)
	DECLARE_TEXTURE_RESOURCE(OktoEyesTexture, "OktoEyes.bmp", false)

	DECLARE_TEXTURE_RESOURCE(AdventureSwordTexture, "sword.bmp", false)
	DECLARE_TEXTURE_RESOURCE(OldManTexture, "guy.bmp", false)
	DECLARE_TEXTURE_RESOURCE(AdventureFireTexture, "AdventureFire.bmp", false)
	DECLARE_TEXTURE_RESOURCE(SpriteSpawnTexture, "SpawnSprite.bmp", false)
	DECLARE_TEXTURE_RESOURCE(EmptyBlockSpriteTexture, "EmptyBlockSprite.bmp", false)
	DECLARE_TEXTURE_RESOURCE(EmptyBlockUSpriteTexture, "EmptyBlockSpriteU.bmp", false)
	DECLARE_TEXTURE_RESOURCE(BrickBounceSpriteTexture, "BrickBounceSprite.bmp", false)
	DECLARE_TEXTURE_RESOURCE(BrickBounceUSpriteTexture, "BrickBounceSpriteU.bmp", false)
	DECLARE_TEXTURE_RESOURCE(EGoombaSpriteTexture, "egoomba.bmp", false)
	DECLARE_TEXTURE_RESOURCE(EGoombaFlagTexture, "egoombasmash.bmp", false)
	DECLARE_TEXTURE_RESOURCE(PlantTexture, "plant.bmp", false)
	DECLARE_TEXTURE_RESOURCE(CoinEffectTexture, "coineffect.bmp", false)
	DECLARE_TEXTURE_RESOURCE(RedCoinEffectTexture, "redcoineffect.bmp", false)
	DECLARE_TEXTURE_RESOURCE(BigMushroomTexture, "bigmushroom.bmp", false)
	DECLARE_TEXTURE_RESOURCE(OneUpMushroomTexture, "oneupmushroom.bmp", false)
	DECLARE_TEXTURE_RESOURCE(MagicMushroomTexture, "magicmushroom.bmp", false)
	DECLARE_TEXTURE_RESOURCE(BrickBreakTexture, "brickbreak.bmp", false)
	DECLARE_TEXTURE_RESOURCE(BrickBreakUTexture, "brickbreakU.bmp", false)
	DECLARE_TEXTURE_RESOURCE(IceBreakTexture, "breakice.png", false)
	DECLARE_TEXTURE_RESOURCE(FlowerTexture, "flower.bmp", false)
	DECLARE_TEXTURE_RESOURCE(FireTexture, "fire.bmp", false)
	DECLARE_TEXTURE_RESOURCE(ExplosionTexture, "explosion.bmp", false)
	DECLARE_TEXTURE_RESOURCE(TurtleTexture, "turtle.bmp", false)
	DECLARE_TEXTURE_RESOURCE(TurtleShellTexture, "turtleshell.bmp", false)
	DECLARE_TEXTURE_RESOURCE(StarTexture, "star.bmp", false);
	DECLARE_TEXTURE_RESOURCE(FlagPoleTexture, "flagpole.bmp", false);
	DECLARE_TEXTURE_RESOURCE(SecretFlagPoleTexture, "secretflagpole.bmp", false);
	DECLARE_TEXTURE_RESOURCE(FlagTexture, "flag.bmp", false);
	DECLARE_TEXTURE_RESOURCE(SecretFlagTexture, "secretflag.bmp", false);
	DECLARE_TEXTURE_RESOURCE(PlayerFlagTexture, "playerflag.bmp", false);
	DECLARE_TEXTURE_RESOURCE(BulletTexture, "bullet.bmp", false);
	DECLARE_TEXTURE_RESOURCE(GuyTexture, "guy.bmp", false);
	DECLARE_TEXTURE_RESOURCE(PlotDeviceTexture, "PlotDevice.bmp", false);
	DECLARE_TEXTURE_RESOURCE(AdventureTriangleTexture, "AdventureTriangle.bmp", false);
	DECLARE_TEXTURE_RESOURCE(SunTexture, "sun.bmp", false);

	BEGIN_DECLARE_ANIMATION_RESORCE(SunAnimation)
	{	
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 128;
		int SpriteHeight = 128;
		
		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };		
		Frames.push_back(new Frame(ResourceManager::SunTexture, SrcRect, 32));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::SunTexture, SrcRect, 16));
		
	}
	END_DECLARE_ANIMATION_RESOURCE(SunAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(SpriteSpawnAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 3; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::SpriteSpawnTexture, SrcRect, 2));
		}

	}
	END_DECLARE_ANIMATION_RESOURCE(SpriteSpawnAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureDoorOpenAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 3; i >= 0; i--)
		{
			int SpriteWidth = 128;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::AdventureDoorCloseTexture, SrcRect, 6));
		}

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureDoorOpenAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureDoorCloseAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 128;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::AdventureDoorCloseTexture, SrcRect, 6));
		}

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureDoorCloseAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(OktoAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 2; i++)
		{
			int SpriteWidth = 128;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::OktoTexture, SrcRect, 6));
		}

	}
	END_DECLARE_ANIMATION_RESOURCE(OktoAnimation)
		
	BEGIN_DECLARE_ANIMATION_RESORCE(OktoSideAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 2; i++)
		{
			int SpriteWidth = 128;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::OktoSideTexture, SrcRect, 6));
		}

	}
	END_DECLARE_ANIMATION_RESOURCE(OktoSideAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureFireAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 2; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::AdventureFireTexture, SrcRect, 6));
		}

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureFireAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureDownAnimation)
	{
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 64;
		int SpriteHeight = 64;

		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };
		Frames.push_back(new Frame(ResourceManager::AdventureDownTexture, SrcRect, 6));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureDownTexture, SrcRect, 6));

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureDownAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureAttackDownAnimation)
	{
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 64;
		int SpriteHeight = 64;

		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };
		Frames.push_back(new Frame(ResourceManager::AdventureAttackDownTexture, SrcRect, 12));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureAttackDownTexture, SrcRect, 1));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureAttackDownTexture, SrcRect, 1));

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureAttackDownAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureAttackUpAnimation)
	{
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 64;
		int SpriteHeight = 64;

		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };
		Frames.push_back(new Frame(ResourceManager::AdventureAttackUpTexture, SrcRect, 12));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureAttackUpTexture, SrcRect, 1));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureAttackUpTexture, SrcRect, 1));

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureAttackUpAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureAttackSideAnimation)
	{
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 64;
		int SpriteHeight = 64;

		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };
		Frames.push_back(new Frame(ResourceManager::AdventureAttackSideTexture, SrcRect, 12));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureAttackSideTexture, SrcRect, 1));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureAttackSideTexture, SrcRect, 1));

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureAttackSideAnimation)
	
	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureUpAnimation)
	{
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 64;
		int SpriteHeight = 64;

		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };
		Frames.push_back(new Frame(ResourceManager::AdventureUpTexture, SrcRect, 6));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureUpTexture, SrcRect, 6));

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureUpAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(AdventureSideAnimation)
	{
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 64;
		int SpriteHeight = 64;

		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };
		Frames.push_back(new Frame(ResourceManager::AdventureSideTexture, SrcRect, 6));

		SrcRect.x += SpriteWidth;
		Frames.push_back(new Frame(ResourceManager::AdventureSideTexture, SrcRect, 6));

	}
	END_DECLARE_ANIMATION_RESOURCE(AdventureSideAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(GoombaGrowAnimation)
	{
		
		int SpriteWidth = 64;
		int SpriteHeight = 128;
		SDL_Rect SmallBodyFrameRect        = { 0,   0, SpriteWidth, SpriteHeight };
		SDL_Rect SmallBodyBigHeadFrameRect = { 64,  0, SpriteWidth, SpriteHeight };
		SDL_Rect BigBodyFrameRect          = { 128, 0, SpriteWidth, SpriteHeight };
		bUseAbsoluteFrameCount = true;		

		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 10));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 1));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 13));		
		
	}
	END_DECLARE_ANIMATION_RESOURCE(GoombaGrowAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(GoombaShrinkAnimation)
	{

		int SpriteWidth = 64;
		int SpriteHeight = 128;
		SDL_Rect SmallBodyFrameRect = { 0,   0, SpriteWidth, SpriteHeight };
		SDL_Rect SmallBodyBigHeadFrameRect = { 64,  0, SpriteWidth, SpriteHeight };
		SDL_Rect BigBodyFrameRect = { 128, 0, SpriteWidth, SpriteHeight };
		bUseAbsoluteFrameCount = true;

		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 16));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 2));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture, BigBodyFrameRect, 4));

	}
	END_DECLARE_ANIMATION_RESOURCE(GoombaShrinkAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(GoombaDuckAnimation)
	{
		int SpriteWidth = 64;
		int SpriteHeight = 128;
		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };		

		Frames.push_back(new Frame(ResourceManager::GoombaDuckTexture, SrcRect, 1));
	}
	END_DECLARE_ANIMATION_RESOURCE(GoombaDuckAnimation)
	

	BEGIN_DECLARE_ANIMATION_RESORCE(FireAnimation)
	{
		int SpriteWidth = 32;
		int SpriteHeight = 32;
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 4; i++)
		{
		
			SDL_Rect SrcRect = { i*SpriteWidth, 0, SpriteWidth, SpriteHeight };
			Frames.push_back(new Frame(ResourceManager::FireTexture, SrcRect, 4));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(FireAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(BrickBreakAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::BrickBreakTexture, SrcRect, 3));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(BrickBreakAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(IceBreakAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::IceBreakTexture, SrcRect, 3));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(IceBreakAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(BrickBreakUAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::BrickBreakUTexture, SrcRect, 3));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(BrickBreakUAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(FlowerAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::FlowerTexture, SrcRect, 3));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(FlowerAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(PlayerGoombaAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::PlayerGoombaTexture, SrcRect, 0.07));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(PlayerGoombaAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(EGoombaAnimation)
	{
		bUseAbsoluteFrameCount = true;
		for (int i = 0; i < 2; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::EGoombaSpriteTexture, SrcRect, 8));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(EGoombaAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(PlayerGoombaTallAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::PlayerGoombaTallTexture, SrcRect, 0.1));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(PlayerGoombaTallAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(EmptyBlockSpriteAnimation)
	{
		for (int i = 0; i < 16; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::EmptyBlockSpriteTexture, SrcRect, 0));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(EmptyBlockSpriteAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(EmptyBlockUSpriteAnimation)
	{
		for (int i = 0; i < 16; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::EmptyBlockUSpriteTexture, SrcRect, 0));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(EmptyBlockUSpriteAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(BrickBounceUSpriteAnimation)
	{
		for (int i = 0; i < 16; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::BrickBounceUSpriteTexture, SrcRect, 0));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(BrickBounceUSpriteAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(BrickBounceSpriteAnimation)
	{
		for (int i = 0; i < 16; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::BrickBounceSpriteTexture, SrcRect, 0));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(BrickBounceSpriteAnimation)
	
	BEGIN_DECLARE_ANIMATION_RESORCE(CoinEffectAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::CoinEffectTexture, SrcRect, 0.05));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(CoinEffectAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(RedCoinEffectAnimation)
	{		

		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::RedCoinEffectTexture, SrcRect, 0.05));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(RedCoinEffectAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(ExplosionAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 3; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::ExplosionTexture, SrcRect, 2));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(ExplosionAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(FireworkAnimation)
	{
		bUseAbsoluteFrameCount = true;

		int SpriteWidth = 64;
		int SpriteHeight = 64;
		SDL_Rect SrcRect;

		for (int i = 0; i < 3; i++)
		{			
			SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::ExplosionTexture, SrcRect, 6));
		}

		SrcRect = { SpriteWidth, 0, SpriteWidth, SpriteHeight };

		Frames.push_back(new Frame(ResourceManager::ExplosionTexture, SrcRect, 6));
	}
	END_DECLARE_ANIMATION_RESOURCE(FireworkAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(TurtleAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 2; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::TurtleTexture, SrcRect, 8));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(TurtleAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(PlantAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 2; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 128;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::PlantTexture, SrcRect, 8));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(PlantAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(StarAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::StarTexture, SrcRect, 2));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(StarAnimation)

public:
	ResourceManager();
	~ResourceManager();	


private:
	static vector <AssetResource*> AllResources;
};

#endif
