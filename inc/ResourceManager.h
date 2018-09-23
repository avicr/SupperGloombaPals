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

#define DECLARE_TEXTURE_RESOURCE(RESOURCE_NAME, FILE_NAME) \
class TEXTURE_RESOURCE_##RESOURCE_NAME : public TextureResource \
{ \
	public: \
	TEXTURE_RESOURCE_##RESOURCE_NAME() : TextureResource(FILE_NAME) {ResourceManager::RESOURCE_NAME = new TEXTURE_RESOURCE_##RESOURCE_NAME(*this);} \
	~TEXTURE_RESOURCE_##RESOURCE_NAME() {}; \
}; \
public : \
static TEXTURE_RESOURCE_##RESOURCE_NAME *RESOURCE_NAME;  \
private : \
static TEXTURE_RESOURCE_##RESOURCE_NAME _##RESOURCE_NAME;  

#define INIT_TEXTURE_RESOURCE(RESOURCE_NAME) \
ResourceManager::TEXTURE_RESOURCE_##RESOURCE_NAME ResourceManager::_##RESOURCE_NAME; \
ResourceManager::TEXTURE_RESOURCE_##RESOURCE_NAME *ResourceManager::RESOURCE_NAME = NULL; 

class TextureResource : public AssetResource
{	
public:
	SDL_Texture* Texture;
	SDL_Rect SrcRect;

public:
	
	TextureResource(string InFileName) : AssetResource(InFileName) { }

	TextureResource(const TextureResource &Other) :
		AssetResource(Other.FileName)
	{		
		SDL_Log("About to load: %s", Other.FileName.c_str());
		SDL_Surface * Image = SDL_LoadBMP((TEXTURE_PATH + FileName).c_str());
		SDL_Log("Loaded: %d", Image);
		SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0, 0xFF));
		Texture = SDL_CreateTextureFromSurface(GetRenderer(), Image);
		SrcRect = { 0, 0, Image->w, Image->h };
		SDL_FreeSurface(Image);
		AddReferenceToManager();
	}

	~TextureResource()
	{
		SDL_DestroyTexture(Texture);
	}
};

class ResourceManager
{
	friend AssetResource;

public:
	DECLARE_TEXTURE_RESOURCE(PlayerGoombaTexture, "Goomba.bmp")
	DECLARE_TEXTURE_RESOURCE(PlayerGoombaTallTexture, "PlayerGoombaTall.bmp")
	DECLARE_TEXTURE_RESOURCE(GoombaGrowTexture, "playergrow.bmp")
	DECLARE_TEXTURE_RESOURCE(GoombaDuckTexture, "goombaduck.bmp")
	DECLARE_TEXTURE_RESOURCE(PlayerGoombaDeadTexture, "goombadead.bmp")

	DECLARE_TEXTURE_RESOURCE(EmptyBlockSpriteTexture, "EmptyBlockSprite.bmp")
	DECLARE_TEXTURE_RESOURCE(EmptyBlockUSpriteTexture, "EmptyBlockSpriteU.bmp")
	DECLARE_TEXTURE_RESOURCE(BrickBounceSpriteTexture, "BrickBounceSprite.bmp")
	DECLARE_TEXTURE_RESOURCE(BrickBounceUSpriteTexture, "BrickBounceSpriteU.bmp")
	DECLARE_TEXTURE_RESOURCE(EGoombaSpriteTexture, "egoomba.bmp")
	DECLARE_TEXTURE_RESOURCE(EGoombaFlagTexture, "egoombasmash.bmp")	
	DECLARE_TEXTURE_RESOURCE(CoinEffectTexture, "coineffect.bmp")
	DECLARE_TEXTURE_RESOURCE(BigMushroomTexture, "bigmushroom.bmp")
	DECLARE_TEXTURE_RESOURCE(OneUpMushroomTexture, "oneupmushroom.bmp")
	DECLARE_TEXTURE_RESOURCE(MagicMushroomTexture, "magicmushroom.bmp")
	DECLARE_TEXTURE_RESOURCE(BrickBreakTexture, "brickbreak.bmp")	
	DECLARE_TEXTURE_RESOURCE(BrickBreakUTexture, "brickbreakU.bmp")
	DECLARE_TEXTURE_RESOURCE(FlowerTexture, "flower.bmp")
	DECLARE_TEXTURE_RESOURCE(FireTexture, "fire.bmp")
	DECLARE_TEXTURE_RESOURCE(ExplosionTexture, "explosion.bmp")
	DECLARE_TEXTURE_RESOURCE(TurtleTexture, "turtle.bmp")
	DECLARE_TEXTURE_RESOURCE(TurtleShellTexture, "turtleshell.bmp")
	DECLARE_TEXTURE_RESOURCE(StarTexture, "star.bmp");
	DECLARE_TEXTURE_RESOURCE(FlagPoleTexture, "flagpole.bmp");
	DECLARE_TEXTURE_RESOURCE(SecretFlagPoleTexture, "secretflagpole.bmp");
	DECLARE_TEXTURE_RESOURCE(FlagTexture, "flag.bmp");
	DECLARE_TEXTURE_RESOURCE(SecretFlagTexture, "secretflag.bmp");
	DECLARE_TEXTURE_RESOURCE(PlayerFlagTexture, "playerflag.bmp");

	
	BEGIN_DECLARE_ANIMATION_RESORCE(GoombaGrowAnimation)
	{
		
		int SpriteWidth = 64;
		int SpriteHeight = 128;
		SDL_Rect SmallBodyFrameRect        = { 0,   0, SpriteWidth, SpriteHeight };
		SDL_Rect SmallBodyBigHeadFrameRect = { 64,  0, SpriteWidth, SpriteHeight };
		SDL_Rect BigBodyFrameRect          = { 128, 0, SpriteWidth, SpriteHeight };
		bUseAbsoluteFrameCount = true;		

		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 10));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyBigHeadFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 1));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 13));		
		
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

		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 16));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 2));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, SmallBodyFrameRect, 4));
		Frames.push_back(new Frame(ResourceManager::GoombaGrowTexture->Texture, BigBodyFrameRect, 4));

	}
	END_DECLARE_ANIMATION_RESOURCE(GoombaShrinkAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(GoombaDuckAnimation)
	{
		int SpriteWidth = 64;
		int SpriteHeight = 128;
		SDL_Rect SrcRect = { 0, 0, SpriteWidth, SpriteHeight };		

		Frames.push_back(new Frame(ResourceManager::GoombaDuckTexture->Texture, SrcRect, 1));
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
			Frames.push_back(new Frame(ResourceManager::FireTexture->Texture, SrcRect, 4));
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

			Frames.push_back(new Frame(ResourceManager::BrickBreakTexture->Texture, SrcRect, 3));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(BrickBreakAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(BrickBreakUAnimation)
	{
		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };
			bUseAbsoluteFrameCount = true;

			Frames.push_back(new Frame(ResourceManager::BrickBreakUTexture->Texture, SrcRect, 3));
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

			Frames.push_back(new Frame(ResourceManager::FlowerTexture->Texture, SrcRect, 3));
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

			Frames.push_back(new Frame(ResourceManager::PlayerGoombaTexture->Texture, SrcRect, 0.07));
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

			Frames.push_back(new Frame(ResourceManager::EGoombaSpriteTexture->Texture, SrcRect, 8));
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

			Frames.push_back(new Frame(ResourceManager::PlayerGoombaTallTexture->Texture, SrcRect, 0.1));
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

			Frames.push_back(new Frame(ResourceManager::EmptyBlockSpriteTexture->Texture, SrcRect, 0));
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

			Frames.push_back(new Frame(ResourceManager::EmptyBlockUSpriteTexture->Texture, SrcRect, 0));
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

			Frames.push_back(new Frame(ResourceManager::BrickBounceUSpriteTexture->Texture, SrcRect, 0));
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

			Frames.push_back(new Frame(ResourceManager::BrickBounceSpriteTexture->Texture, SrcRect, 0));
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

			Frames.push_back(new Frame(ResourceManager::CoinEffectTexture->Texture, SrcRect, 0.05));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(CoinEffectAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(ExplosionAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 3; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::ExplosionTexture->Texture, SrcRect, 2));
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

			Frames.push_back(new Frame(ResourceManager::ExplosionTexture->Texture, SrcRect, 6));
		}

		SrcRect = { SpriteWidth, 0, SpriteWidth, SpriteHeight };

		Frames.push_back(new Frame(ResourceManager::ExplosionTexture->Texture, SrcRect, 6));
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

			Frames.push_back(new Frame(ResourceManager::TurtleTexture->Texture, SrcRect, 8));
		}
	}
	END_DECLARE_ANIMATION_RESOURCE(TurtleAnimation)

	BEGIN_DECLARE_ANIMATION_RESORCE(StarAnimation)
	{
		bUseAbsoluteFrameCount = true;

		for (int i = 0; i < 4; i++)
		{
			int SpriteWidth = 64;
			int SpriteHeight = 64;
			SDL_Rect SrcRect = { i * SpriteWidth, 0, SpriteWidth, SpriteHeight };

			Frames.push_back(new Frame(ResourceManager::StarTexture->Texture, SrcRect, 2));
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
