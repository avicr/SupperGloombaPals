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
	void AddScore(int Amount, int PointSpriteX = -1, int PointSpriteY = -1, bool bIsOneUp = false);
	void IncreaseStompCounter(int Amount);
	void SetColorModForAllTextures(SDL_Color ColorMod);
	void TakeDamage();
	virtual void Render(SDL_Renderer* Renderer, int ResourceNum = 0);
	void UpdateDyingAnimation();
	void DrawHUD();
	void BeginLevel();
	int GetHeight() { return Rect.h; }
	int GetLives() { return Lives; }
	bool IsDead();
	bool IsDying();
	void BeginDie();
	int GetTripLevel() {return TripLevel;}
	void Launch();
	void GrabFlagPole(class FlagPoleSprite* FlagPole);
	void UpdateFlagPoleAnimation();
	void OnFlagFinished();
	bool HasExitedLevel();	

	//virtual SDL_Rect GetScreenSpaceCustomRect();

	PlayerSprite();

protected:
	bool bDrawHUD;
	bool bExitedLevel;
	int EndOfLevelCountdown;
	int ExitLevelFailSafeCountdown;
	int ExitLevelX;
	bool bExitingLevel;
	int JumpOffPoleCountDown;
	int FlagStopY;
	bool bRidingFlagPole;
	int StompCount;	
	int TripLevel;
	bool bSpriteVisible;
	ePowerUpState PowerUpState;
	WarpSequenceInfo WarpSeq;

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
	
	void StopJumping();
	bool HandleVerticalMovement();
	
};
