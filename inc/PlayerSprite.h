#pragma once

#include "PhysicsSprite.h"
#include "TMXMap.h"

enum ePowerUpState
{
	POWER_NONE = 0,
	POWER_BIG,
	POWER_FLOWER	
};

struct WarpSequenceInfo
{	
	WarpEntrance Entrance;
	WarpExit Exit;
	int FrameCount;
	bool bWarpComplete;
	bool bWarpExitComplete;	
};

class PlayerSprite : public PhysicsSprite
{
public:
	virtual void Tick(double DeltaTime);
	virtual void HandleInput(double DeltaTime);
	virtual void TickAnimation(double DeltaTime);
	virtual void UpdateStarAnimation();

	virtual void AdventureTick(double DeltaTime);
	virtual void AdventureHandleInput(double DeltaTime);

	bool IsRightPressed(const Uint8 *state);
	bool IsLeftPressed(const Uint8 *state);
	bool IsUpPressed(const Uint8 *state);
	bool IsDownPressed(const Uint8 *state);

	bool IsButton1Pressed(const Uint8 *state);
	bool IsButton2Pressed(const Uint8 *state);
	void HandleJump();
	void Stomp(Sprite* Other);
	void Kick(Sprite* Other);
	void SetPowerUpState(ePowerUpState NewPowerUpState);	
	ePowerUpState GetPowerUpState() { return PowerUpState; }
	virtual bool Interact(EnemySprite* Enemy);
	virtual void Interact(ItemSprite* Item);
	virtual void OnGetBig();
	virtual void OnGetSmall();
	virtual void OnGetFire();
	bool IsChangingSize();
	void Duck();
	void StandUp();
	void ShootFire();
	void OnFireBallDestroy();
	void StartWarpSequence(WarpEntrance Entrance, WarpExit Exit);
	bool IsWarping();
	void CheckControlCollision();
	void CheckWarpCollision();
	void UpdateWarpSequence();
	void UpdateWarpExitSequence();
	virtual void CheckCollisionWithSprites();
	void AddCoins(int Amount);
	void AddRedCoins(int Amount, int TileX, int TileY);
	void AddScore(int Amount, int PointSpriteX = -1, int PointSpriteY = -1, bool bIsOneUp = false);
	void IncreaseStompCounter(int Amount);
	void SetColorModForAllTextures(SDL_Color ColorMod);
	virtual void TakeDamage();
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum = 0);
	void UpdateDyingAnimation();
	void DrawHUD();
	void BeginLevel();
	void EndLevel();
	int GetHeight() { return Rect.h; }
	int GetLives() { return Lives; }
	bool IsDead();
	bool IsDying();
	void BeginDie();
	int GetTripLevel() {return TripLevel;}
	void Launch(double InVelocityY = -16);
	void GrabFlagPole(class FlagPoleSprite* FlagPole);
	void UpdateFlagPoleAnimation();
	void OnFlagFinished();
	bool HasExitedLevel();	
	bool IsGrowing() { return bGrowing; }
	bool IsShrinking() { return bShrinking; }
	void SetFrozen(bool bInFrozen) 
	{
		bFrozen = bInFrozen; 
		if (bFrozen)
		{
			VelocityX = 0;
		}
	}

	virtual SDL_Rect GetScreenSpaceCustomRect();

	bool HasSword() {return bHasSword;}
	PlayerSprite();	

protected:
	class AdventureSwordAttack* CurrentAdventureAttack;
	int HP;
	bool bStompedLastFrame;
	bool bFrozen;
	bool bDrawHUD;
	bool bExitedLevel;
	int EndOfLevelCountdown;
	int ExitLevelFailSafeCountdown;
	int ExitLevelX;
	bool bExitingLevel;
	int JumpOffPoleCountDown;
	int FlagStopY;
	bool bRidingFlagPole;
	int SwordAnimationCount;
	int TriforceAnimationCount;
	int StompCount;	
	int TripLevel;
	bool bSpriteVisible;
	bool bAdventureAttacking;
	ePowerUpState PowerUpState;
	WarpSequenceInfo WarpSeq;

	bool bHasSword;
	int NumFireBalls;
	const Uint8 *LastKeyboardState;
	float StartJumpVelocity;
	float JumpInitialVelocityX;
	bool bIsRunning;
	bool bIsJumping;	
	bool bFirstFrameJump;
	bool bButtonPreviouslyPressed[3];
	int Coins;	
	int Lives;
	int Score;
	bool bGrowing;
	bool bShrinking;
	bool bDucking;
	bool bWarping;
	int StarCountdown;
	int InvincibleCount;
	int RunReleaseCount;
	int DyingCount;
	bool bDead;
	eDirection EjectionDirection;
	int NumRedCoins;

	bool bIsAdventure;
	int AdventureMoveIndex;
	Sprite* HeldItemSprite;
	
	void StopJumping();
	bool HandleVerticalMovement();
	void UpdateGetSwordAnimation();
	void UpdateGetTriforceAnimation();
	bool SubstepXDirection(int NumPixels);
	bool SubstepYDirection(int NumPixels);
	void UpdateAdventureAnimation();
	void AdventureTakeDamage();
};
