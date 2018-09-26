#ifndef SPRITE_H
#define SPRITE_H

#ifdef _WIN32
#include <SDL.h>
#else
#include "SDL2/SDL.h"
#endif

#include <vector>
#include <string>
#include "../inc/ResourceManager.h"

using namespace std;

enum eRenderLayer
{
	RENDER_LAYER_BEHIND_BG = 0,
	RENDER_LAYER_BEHIND_FG,
	RENDER_LAYER_TOP,
	RENDER_LAYER_ABOVE_ALL

};

enum eMovingFlags
{
	MOVING_NONE  = 0,
	MOVING_UP    = 1,
	MOVING_DOWN  = 2,
	MOVING_LEFT  = 4,
	MOVING_RIGHT = 8
};

class Sprite
{
protected:
	SDL_Color CollisionRenderColor;
	SDL_Rect CollisionRect;
	double CountDown;
	SDL_RendererFlip Flip;
	// Screen region to render the sprite to
	SDL_Rect Rect;

	// Pointer to the current frame in the animation
	SDL_Texture* Texture;
	SDL_Texture* Texture2;

	AnimInfo AnimData;	

	bool bPendingDelete;	

	double PosX;
	double PosY;
	double Scale;

	double VelocityX;
	double VelocityY;

	int MovingFlags;

	// If true, do not adjust for the scroll offset when drawing
	bool bDrawScreenSpace;
	bool bDeleteWhenNotVisible;

	// Used for the window hack!
	bool bForceDrawWhenNotInWindow;

	eRenderLayer RenderLayer;
public:
	Sprite();
	Sprite(SDL_Texture *InTexture);
	void SetFlip(SDL_RendererFlip);
	void SetMovingFlags(eMovingFlags NewFlags);	
	int GetMovingFlags();
	virtual ~Sprite() {};
	virtual void Tick(double DeltaTime);
	virtual void TickAnimation(double DeltaTime);
	virtual void HandleInput(double DeltaTime);
	virtual void Interact(Sprite *OtherSprite) {};
	virtual bool Interact(class EnemySprite *OtherSprite) { return true; };
	virtual void Interact(class PlayerSprite *OtherSprite) {};
	virtual void Interact(class ItemSprite *OtherSprite) {};
	virtual SDL_Rect GetScreenSpaceCollisionRect();	

	// Returns the collision rect with width and height set to multiples of 64
	virtual SDL_Rect GetScreenSpaceRect();
	virtual SDL_Rect GetScreenSpaceCustomRect();
	virtual SDL_Rect GetMapSpaceCollisionRect();
	void SetTexture(SDL_Texture* Texture);
	void SetPosition(double NewX, double NewY);
	void SetWidth(int NewWidth);
	void SetHeight(int NewHeight);
	virtual int GetWidth() { return Rect.w; }
	virtual int GetHeight() { return Rect.h; }
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum = 0);
	void RenderCollision(SDL_Renderer* Renderer);
	void PlayAnimation(AnimationResource *Anim, bool bLoop = true);
	void SetAnimationPlayRate(double Rate);
	void UpdateAnimationData();
	void StopAnimation();
	bool GetPendingDelete();
	void Delete();
	virtual int GetScoreWorth() { return 0; }
	double GetPosX() { return PosX; }
	double GetPosY() { return PosY; }
	bool InMapWindow(SDL_Point Offset = { 0, 0 });
	virtual void GetStomped();
	virtual void GetKicked(PlayerSprite* Player);
	virtual bool IsStompable();
	virtual bool IsKickable();
	virtual void GetBricked(int TileX, int TileY);
	virtual void GetFired() {}	
	virtual void CheckCollisionWithSprites();
	virtual bool IsOnGround();
	virtual bool IsInteractable() { return !bPendingDelete; }
	virtual eRenderLayer GetRenderLayer() { return RenderLayer; }
	virtual SDL_Texture* GetTexture(int ResourceNum = 0);
};

#endif
