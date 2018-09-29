#include "../inc/ResourceManager.h"

vector <AssetResource*> ResourceManager::AllResources;

INIT_TEXTURE_RESOURCE(PlayerGoombaTexture)
INIT_TEXTURE_RESOURCE(PlayerGoombaTallTexture)
INIT_TEXTURE_RESOURCE(EmptyBlockSpriteTexture)
INIT_TEXTURE_RESOURCE(EmptyBlockUSpriteTexture)
INIT_TEXTURE_RESOURCE(BrickBounceSpriteTexture)
INIT_TEXTURE_RESOURCE(BrickBounceUSpriteTexture)
INIT_TEXTURE_RESOURCE(EGoombaSpriteTexture)
INIT_TEXTURE_RESOURCE(EGoombaFlagTexture)
INIT_TEXTURE_RESOURCE(CoinEffectTexture)
INIT_TEXTURE_RESOURCE(RedCoinEffectTexture)
INIT_TEXTURE_RESOURCE(BigMushroomTexture)
INIT_TEXTURE_RESOURCE(OneUpMushroomTexture)
INIT_TEXTURE_RESOURCE(MagicMushroomTexture)
INIT_TEXTURE_RESOURCE(BrickBreakTexture)
INIT_TEXTURE_RESOURCE(BrickBreakUTexture)
INIT_TEXTURE_RESOURCE(GoombaGrowTexture)
INIT_TEXTURE_RESOURCE(GoombaDuckTexture)
INIT_TEXTURE_RESOURCE(FlowerTexture)
INIT_TEXTURE_RESOURCE(FireTexture)
INIT_TEXTURE_RESOURCE(ExplosionTexture)
INIT_TEXTURE_RESOURCE(TurtleTexture)
INIT_TEXTURE_RESOURCE(TurtleShellTexture)
INIT_TEXTURE_RESOURCE(StarTexture)
INIT_TEXTURE_RESOURCE(PlayerGoombaDeadTexture)
INIT_TEXTURE_RESOURCE(FlagPoleTexture)
INIT_TEXTURE_RESOURCE(SecretFlagPoleTexture)
INIT_TEXTURE_RESOURCE(FlagTexture)
INIT_TEXTURE_RESOURCE(SecretFlagTexture)
INIT_TEXTURE_RESOURCE(PlayerFlagTexture)
INIT_TEXTURE_RESOURCE(EventBoxTexture)

INIT_ANIMATION_RESOURCE(PlayerGoombaAnimation)
INIT_ANIMATION_RESOURCE(PlayerGoombaTallAnimation)
INIT_ANIMATION_RESOURCE(EmptyBlockSpriteAnimation);
INIT_ANIMATION_RESOURCE(EmptyBlockUSpriteAnimation);
INIT_ANIMATION_RESOURCE(BrickBounceSpriteAnimation);
INIT_ANIMATION_RESOURCE(BrickBounceUSpriteAnimation);
INIT_ANIMATION_RESOURCE(EGoombaAnimation)
INIT_ANIMATION_RESOURCE(CoinEffectAnimation)
INIT_ANIMATION_RESOURCE(RedCoinEffectAnimation)
INIT_ANIMATION_RESOURCE(BrickBreakAnimation)
INIT_ANIMATION_RESOURCE(BrickBreakUAnimation)
INIT_ANIMATION_RESOURCE(GoombaGrowAnimation)
INIT_ANIMATION_RESOURCE(GoombaDuckAnimation)
INIT_ANIMATION_RESOURCE(FlowerAnimation)
INIT_ANIMATION_RESOURCE(FireAnimation)
INIT_ANIMATION_RESOURCE(ExplosionAnimation)
INIT_ANIMATION_RESOURCE(TurtleAnimation)
INIT_ANIMATION_RESOURCE(StarAnimation)
INIT_ANIMATION_RESOURCE(GoombaShrinkAnimation)
INIT_ANIMATION_RESOURCE(FireworkAnimation)

ResourceManager::ResourceManager()
{
	
}

ResourceManager::~ResourceManager()
{
	for (int i = 0; i < AllResources.size(); i++)
	{
		if (AllResources[i])
		{
			delete AllResources[i];
			AllResources[i] = NULL;
		}
	}
}
