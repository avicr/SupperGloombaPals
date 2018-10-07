#include "../inc/PlayerSprite.h"
#include "../inc/SpriteList.h"
#include "../inc/EmptyBlockSprite.h"
#include "../inc/TMXMap.h"
#include "../inc/ItemSprite.h"
#include "../inc/SimpleSprites.h"
#include "../inc/Game.h"

int oldx;
int oldy;

void PlayerSprite::Tick(double DeltaTime)
{	
	// Don't do anything if the level isn't playing
	if (TheGame->GetGameState() > STATE_RIDING_FLAG_POLE)
	{
		return;
	}

	oldx = PosX;
	oldy = PosY;

	bool bBumpedHead = false;
	double VerticalScrollDelta = PosY;
	// Skip physics tick!
	Sprite::Tick(DeltaTime);	
	
	// We have entered the castle, wait a few frames
	if (EndOfLevelCountdown && bExitingLevel)
	{
		EndOfLevelCountdown--;

		if (EndOfLevelCountdown == 0)
		{
			bExitedLevel = true;
			TheGame->OnLevelComplete();			
		}
	}

	if (JumpOffPoleCountDown)
	{
		JumpOffPoleCountDown--;

		if (JumpOffPoleCountDown == 0)
		{
			bExitingLevel = true;
			SetFlip(SDL_FLIP_NONE);
			Mix_PlayMusic(WinMusic, 0);
			VelocityX = MAX_WALK_SPEED;
		}

		return;
	}

	if (bRidingFlagPole)
	{
		UpdateFlagPoleAnimation();

		Rect.x = PosX;
		Rect.y = PosY;
		return;
	}

	if (DyingCount)
	{
		UpdateDyingAnimation();
		return;
	}

	if (TheMap->GetSecondsLeft() <= 0)
	{
		BeginDie();
	}

	// Check for pit death
	int KillY = TheMap->GetKillY();
	if (!IsWarping() && KillY != -1 && PosY >= KillY)
	{
		bSpriteVisible = false;
		BeginDie();
	}

	if (InvincibleCount > 0)
	{
		InvincibleCount--;

		if (InvincibleCount == 0 || InvincibleCount % 2 == 0)
		{
			bSpriteVisible = true;
		}
		else
		{
			bSpriteVisible = false;
		}
	}

	if (StarCountdown > 0)
	{		
		UpdateStarAnimation();
	}	

	if (RunReleaseCount > 0)
	{
		RunReleaseCount--;
	}

	if (bWarping)
	{
		UpdateWarpSequence();		
		return;
	}

	if (bGrowing)
	{
		return;
	}

	if (bShrinking)
	{	
		return;
	}

	// Launch if we stomped last frame
	if (bStompedLastFrame)
	{
		Launch();
	}
	bStompedLastFrame = false;

	bool bCollided = false;
	bool bWasInAirLastFrame = bIsInAir;	

	bIsInAir = !IsOnGround();
	
	if (!bIsInAir)
	{
		StompCount = 0;
	}

	bool bFirstFrameInAir = bIsInAir && !bWasInAirLastFrame;

	if (!bIsJumping && bIsInAir && !bWasInAirLastFrame)
	{
		//VelocityY = BASE_FALL_VELOCITY;
		JumpInitialVelocityX = VelocityX; 		
	}

	if (!bIsInAir || bFirstFrameInAir)
	{
		if ((MovingFlags & MOVING_RIGHT) && (MovingFlags & MOVING_DOWN) == 0)
		{			
			if (!bButtonPreviouslyPressed[1] || RunReleaseCount)
			{
				VelocityX += WALKING_ACCELERATION;
			}
			else
			{
				VelocityX += RUNNING_ACCELERATION;
			}
		}
		// If we were moving right but aren't now
		else if (VelocityX > 0)
		{
			// If the user isn't pressing left or right		
			if (!(MovingFlags & MOVING_LEFT) || MovingFlags & MOVING_DOWN)
			{
				VelocityX -= RELEASE_DECELERATION;
			}
			else
			{
				VelocityX -= SKID_DECELERATION;
			}

			if (VelocityX < 0)
			{
				VelocityX = 0;
			}
		}

		if (MovingFlags & MOVING_LEFT && (MovingFlags & MOVING_DOWN) == 0)
		{
			if (!bButtonPreviouslyPressed[1] || RunReleaseCount)
			{
				VelocityX += -WALKING_ACCELERATION;
			}
			else
			{
				VelocityX += -RUNNING_ACCELERATION;
			}
		}
		// If we were moving left but aren't now
		else if (VelocityX < 0)
		{
			// If the user isn't pressing left or right		
			if (!(MovingFlags & MOVING_RIGHT) || MovingFlags & MOVING_DOWN)
			{
				VelocityX += RELEASE_DECELERATION;
			}
			else
			{
				VelocityX += SKID_DECELERATION;
			}

			if (VelocityX > 0)
			{
				VelocityX = 0;
			}
		}


		if (!bButtonPreviouslyPressed[1])
		{
			if (VelocityX > MAX_WALK_SPEED)
			{
				VelocityX -= RELEASE_DECELERATION;
			}

			if (VelocityX < -MAX_WALK_SPEED)
			{
				VelocityX += RELEASE_DECELERATION;
			}
		}
		else
		{
			if (VelocityX > MAX_RUN_SPEED)
			{
				VelocityX = MAX_RUN_SPEED;
			}

			if (VelocityX < -MAX_RUN_SPEED)
			{
				VelocityX = -MAX_RUN_SPEED;
			}
		}
	}
	else
	{
		HandleJump();
	}

	// Don't mess with animation play rates if we are growing
	if (!bGrowing && !bShrinking)
	{						
		if (VelocityX != 0 && !bIsInAir)
		{
			if (fabs(VelocityX) < MAX_WALK_SPEED / 2)
			{
				SetAnimationPlayRate(0.5);
			}
			else if (fabs(VelocityX) < MAX_WALK_SPEED)
			{
				SetAnimationPlayRate(0.75);
			}
			else if (fabs(VelocityX) <= MAX_RUN_SPEED)
			{
				SetAnimationPlayRate(1.5);
			}
		}
		else
		{
			SetAnimationPlayRate(0);
		}
	}

	if (fabs(VelocityX) >= MIN_WALKING_VELOCITY || bIsInAir)
	{
		int NumPixelsToMove = floor(fabs(VelocityX));
		SDL_Rect NewRect = GetScreenSpaceCollisionRect();
		for (int x = 0; x < NumPixelsToMove; x++)
		{
			NewRect.x += VelocityX > 0 ? 1 : -1;

			if (EndOfLevelCountdown == 0 &&  bExitingLevel && Rect.x >= ExitLevelX)
			{								
				bSpriteVisible = false;
				EndOfLevelCountdown = 40;
				VelocityX = 0;
				VelocityY = 0;
			}

			if (TheMap->CheckCollision(NewRect))
			{
				// Cap max speed when colliding with a wall
				bCollided = true;
				VelocityX = 0;
				
				break;
			}
			else
			{
				PosX += VelocityX > 0 ? 1 : -1;
				
				if (PosX - TheMap->GetScrollX() > PLAYER_SCROLL_START && VelocityX > 0)
				{
					if (Rect.x - TheMap->GetScrollX() < 512 - 86)
					{
						TheMap->AdjustScrollX(0.8);
					}
					else
					{
						TheMap->AdjustScrollX(1);
					}
				}

				if (PosX - TheMap->GetScrollX() < PLAYER_SCROLL_START + 64 && VelocityX < 0)
				{
					if (Rect.x - TheMap->GetScrollX() > 512 - 86)
					{
						TheMap->AdjustScrollX(-0.8);
					}
					else
					{
						TheMap->AdjustScrollX(-1);
					}
				}
			}
		}
	}
	
	if (!bIsJumping)
	{
		if (bIsInAir)
		{
			VelocityY += BASE_FALL_VELOCITY;

			if (VelocityY > 18)
			{
				VelocityY = 18;
			}
		}
		else/* if ((int)floor(PosY) % 64 == 0)*/
		{			
			VelocityY = 0;			
		}
	}
	else if (!bFirstFrameJump)
	{
		if (!bButtonPreviouslyPressed[0])
		{
			if (JumpInitialVelocityX >= VERY_FAST_VELOCITY_X)
			{
				VelocityY += BASE_FALL_VELOCITY_VERY_FAST;
			}
			else if (JumpInitialVelocityX >= FAST_AIR_SPEED)
			{
				VelocityY += BASE_FALL_VELOCITY_FAST;
			}
			else
			{
				VelocityY += BASE_FALL_VELOCITY;
			}			
		}
		else
		{
			if (JumpInitialVelocityX >= VERY_FAST_VELOCITY_X)
			{
				VelocityY += HOLDING_A_JUMP_FALL_VELOCITY_VERY_FAST;
			}
			else if (JumpInitialVelocityX >= FAST_AIR_SPEED)
			{
				VelocityY += HOLDING_A_JUMP_FALL_VELOCITY_FAST;
			}
			else
			{
				VelocityY += HOLDING_A_JUMP_FALL_VELOCITY;
			}			
		}

		if (VelocityY > 16)
		{
			VelocityY = 16;
		}
	}
		
	if (VelocityY != 0)
	{
		bBumpedHead = HandleVerticalMovement();
	}
	
	// If we're exiting the level, cap the walk speed no matter what
	if (bExitingLevel)
	{
		if (VelocityX > MAX_WALK_SPEED)
		{
			VelocityX = MAX_WALK_SPEED;
		}		
	}
	Rect.x = PosX;// -TheMap->GetScrollX();
	Rect.y = PosY;

	VerticalScrollDelta -= PosY;

	bool bScrollingDown = VelocityY > 0 && Rect.y + Rect.h - TheMap->GetScrollY() > 816;
	bool bScrollingUp = VelocityY < 0 && Rect.y - TheMap->GetScrollY() < 50;

	if (bScrollingDown || bScrollingUp)
	{
		TheMap->AdjustScrollY(-VerticalScrollDelta);
	}	

	// Wall ejection!
	vector<TileInfo> HitTiles;
	bool bCollidedDuringFinalCheck = TheMap->CheckCollision(GetScreenSpaceCollisionRect(), HitTiles);

	for (int i = 0; i < HitTiles.size(); i++)
	{
		if (TheMap->GetMetaTileType(HitTiles[i].Location.x, HitTiles[i].Location.y) == TILE_COIN)
		{
			TheMap->SetForegroundTile(HitTiles[i].Location.x, HitTiles[i].Location.y, -1);
			TheMap->SetMetaLayerTile(HitTiles[i].Location.x, HitTiles[i].Location.y, TILE_NONE);

			// Prevent the coin from firing multiple times
			if (!ThePlayer->IsChangingSize())
			{
				AddCoins(1);
				AddScore(200);
			}
		}
		// TODO: Red coin
		else if (TheMap->GetMetaTileType(HitTiles[i].Location.x, HitTiles[i].Location.y) == TILE_RED_COIN)
		{
			TheMap->SetForegroundTile(HitTiles[i].Location.x, HitTiles[i].Location.y, -1);
			TheMap->SetMetaLayerTile(HitTiles[i].Location.x, HitTiles[i].Location.y, TILE_NONE);

			// Prevent the coin from firing multiple times
			if (!ThePlayer->IsChangingSize())
			{
				AddRedCoins(1, HitTiles[i].Location.x, HitTiles[i].Location.y);
				AddScore(1000);
			}
		}

	}
	SDL_Rect ScreenSpaceColRect = GetScreenSpaceCustomRect();
	ScreenSpaceColRect.y++;

	bool PastLeftEdge = PosX - TheMap->GetScrollX() < -4 && !bIsJumping;

	bool bOriginalCheck = ((GetMapSpaceCollisionRect().x >= TheMap->GetScrollX()) || (VelocityY >= 0 && !bBumpedHead)) && bCollidedDuringFinalCheck;
	bool bNewCheck = Rect.y % 64 == 0  && GetMapSpaceCollisionRect().x >= TheMap->GetScrollX() && !bCollided && bCollidedDuringFinalCheck;
	if (bOriginalCheck || PastLeftEdge)
	{
		//if ((GetMapSpaceCollisionRect().x < TheMap->GetScrollX() && Flip & SDL_FLIP_HORIZONTAL) || GetMapSpaceCollisionRect().x >= TheMap->GetScrollX())
		//{
		//	PosX += Flip == SDL_FLIP_HORIZONTAL ? 3 : -3;
		//	Rect.x = PosX;
		//}
		//else
		//{
		//	// If we're stuck at the scroll point of the map, force a right side ejection
		//	PosX += 3;
		//	Rect.x = PosX;
		//}

		/*if ((GetMapSpaceCollisionRect().x < TheMap->GetScrollX() && Flip & SDL_FLIP_HORIZONTAL) || GetMapSpaceCollisionRect().x >= TheMap->GetScrollX())
		{
			PosX += Flip == SDL_FLIP_HORIZONTAL ? 3 : -3;
			Rect.x = PosX;
		}
		else*/
		{
			// Go left if there is an emtpy tile to the left
			if (EjectionDirection == DIRECTION_LEFT || (EjectionDirection != DIRECTION_RIGHT && PosX - TheMap->GetScrollX() > 0 && HitTiles.size() > 0 && !TheMap->IsCollidableTile(HitTiles[0].MetaTileType, HitTiles[0].Location.x, HitTiles[0].Location.y)))
			{
				PosX -= 3;
				EjectionDirection = DIRECTION_LEFT;
			}
			else
			{
				PosX += 3;
				EjectionDirection = DIRECTION_RIGHT;
			}
			Rect.x = PosX;
		}
	}
	else
	{
		EjectionDirection = DIRECTION_NONE;
	}

	CheckCollisionWithSprites();
}	

void PlayerSprite::UpdateStarAnimation()
{	
	StarCountdown--;

	// Start the music 2 seconds before the star ends
	if (StarCountdown == 120)
	{
		SetColorModForAllTextures({ 0, 0, 0 });
		
		Mix_PlayMusic(TheGame->GetMusic(), 1);			
		
	}
	else if (StarCountdown % 15 == 0)
	{
		SetColorModForAllTextures({ 255, 242, 44 });
	}
	else if (StarCountdown % 10 == 0)
	{
		SetColorModForAllTextures({ 125, 60, 255 });
	}
	else if (StarCountdown % 5 == 0)
	{
		SetColorModForAllTextures({ 255, 128, 140 });
	}

	if (StarCountdown == 0)
	{				
		if (PowerUpState < POWER_FLOWER)
		{
			SetColorModForAllTextures({ 255, 255, 255 });
		}
		else
		{
			SetColorModForAllTextures({ 255, 128, 128 });
		}
	}
}

bool PlayerSprite::Interact(EnemySprite* Enemy)
{	
	if (Enemy->IsDying() || DyingCount)
	{
		return true;
	}

	SDL_Rect EnemyCollision = Enemy->GetScreenSpaceCollisionRect();
	SDL_Rect ResultRect;

	SDL_Rect CollisionRect = GetScreenSpaceCollisionRect();

	// Intersect is not inclusive, so widen the collision box
	EnemyCollision.x -= 1;
	EnemyCollision.w += 2;
	EnemyCollision.y -= 1;
	EnemyCollision.h += 2;

	if (SDL_IntersectRect(&EnemyCollision, &CollisionRect, &ResultRect))
	{
		if (StarCountdown)
		{
			Enemy->GetStarred(PosX / 64 * 64, PosY / 64 * 64);
			AddScore(100, Enemy->GetPosX(), Enemy->GetPosY());
		}
		// If the bottom of the player's foot is ontop or above the enemy, kill it
		else if (Enemy->IsStompable() && VelocityY > 0 && EnemyCollision.y + EnemyCollision.h >= CollisionRect.y + 1)
		{
			Stomp(Enemy);	
			return false;
		}
		else if (Enemy->IsKickable())
		{
			Kick(Enemy);
		}
		else if (InvincibleCount <= 0 && !IsWarping())
		{			
			TakeDamage();
			SDL_Log("You got hit sucker");			
		}
	}

	return true;
}

void PlayerSprite::Interact(ItemSprite* Item)
{
	if (Item->GetPendingDelete())
	{
		return;
	}

	bool bIsOneUp = false;
	eItemType ItemType = Item->GetItem();
	Item->GetCollected();	

	if (ItemType == ITEM_MUSHROOM && PowerUpState == POWER_NONE)
	{
		Mix_PlayChannel(CHANNEL_POWER_UP, PowerUpGetSound, 0);
		OnGetBig();		
	}
	if (ItemType == ITEM_FLOWER)
	{
		Mix_PlayChannel(CHANNEL_POWER_UP, PowerUpGetSound, 0);

		if (PowerUpState == POWER_NONE)
		{
			OnGetBig();
		}
		else
		{
			OnGetFire();
		}
	}
	else if (ItemType == ITEM_ONE_UP)
	{
		Lives++;
		bIsOneUp = true;
		Mix_PlayChannel(CHANNEL_ONE_UP, OneUpSound, 0);
	}
	else if (ItemType == ITEM_STAR)
	{
		StarCountdown = NUM_STAR_FRAMES;
		Mix_PlayMusic(StarmanMusic, -1);
	}
	else if (ItemType == ITEM_MAGIC_MUSHROOM)
	{
		TripLevel++;
		
		if (TripLevel == 3)
		{
			bWorldSmear = true;
		}

		if (TripLevel == 4)
		{
			// TODO:
			// PlayNewMusic	ee
			bWorldSmear = false;
			bRenderCollision = true;
		}
		
		Mix_PlayChannel(CHANNEL_POWER_UP, PowerUpGetSound, 0);
	}
	
	AddScore(1000, Item->GetPosX(), Item->GetPosY(), bIsOneUp);
	
	// Delete the item that we just consumed
	ItemSprites.DeletePendingActors();
}

void PlayerSprite::OnGetBig()
{	
	PowerUpState = POWER_BIG;
	bGrowing = true;
	SetWidth(64);
	SetHeight(128);	
	Rect.y -= 64;
	PlayAnimation(GResourceManager->GoombaGrowAnimation, false);
	//PlayAnimation(GResourceManager->GoombaShrinkAnimation, false);
	
}

void PlayerSprite::OnGetSmall()
{
	PowerUpState = POWER_NONE;
	bShrinking = true;
	InvincibleCount = NUM_INVINCIBLE_FRAMES;
		
	PlayAnimation(GResourceManager->GoombaShrinkAnimation, false);	
	Mix_PlayChannel(CHANNEL_PIPE, PipeSound, 0);
}

void PlayerSprite::OnGetFire()
{
	PowerUpState = POWER_FLOWER;
	SetColorModForAllTextures({ 255, 128, 128 });
}

void PlayerSprite::HandleInput(double DeltaTime)
{
	MovingFlags = MOVING_NONE;


	if (EndOfLevelCountdown || bFrozen)
	{
		return;
	}
	// If we're done with flag stuff, move right!
	if (bExitingLevel && JumpOffPoleCountDown == 0)
	{
		MovingFlags = MOVING_RIGHT;
		return;
	}

	if (bWarping)
	{
		return;
	}

	if (bGrowing || bShrinking || DyingCount)
	{
		return;
	}

	const Uint8 *state = SDL_GetKeyboardState(NULL);
	LastKeyboardState = state;
	SDL_JoystickUpdate();	
	bool bPlayerMoving = false;	
	bIsRunning = false;
	bFirstFrameJump = false;
	
	/*if (Joy)
	{
	SDL_Log("Joy axis 2: %d", SDL_JoystickGetAxis(Joy, 2));
	SDL_Log("Joy axis 1: %d", SDL_JoystickGetAxis(Joy, 1));
	SDL_Log("Joy axis 0: %d", SDL_JoystickGetAxis(Joy, 0));
	}*/
		
	if (IsRightPressed(state))
	{
		if (!bIsInAir && !IsDownPressed(state))
		{
			SetFlip(SDL_FLIP_NONE);			
		}
		MovingFlags |= MOVING_RIGHT;
		bPlayerMoving = true;
		
	}
	else if (IsLeftPressed(state))
	{
		if (!bIsInAir && !IsDownPressed(state))
		{			
			SetFlip(SDL_FLIP_HORIZONTAL);
		}
		MovingFlags |= MOVING_LEFT;
		bPlayerMoving = true;
		
	}	
	
	if (IsUpPressed(state))
	{
		MovingFlags |= MOVING_UP;
		bPlayerMoving = true;
	}
	else if (IsDownPressed(state))
	{
		MovingFlags |= MOVING_DOWN;
		bPlayerMoving = true;

		if (!bIsInAir)
		{
			// If we're ducking and holding left or right, stand up cause the original did this
			if (MovingFlags & MOVING_LEFT || MovingFlags & MOVING_RIGHT)
			{
				StandUp();
			}
			else
			{
				Duck();
			}			
		}
	}
	else if ((!IsDownPressed(state) && bDucking && !bIsInAir))  // We're standing up 
	{
		StandUp();
	}

	bool bPressingButton1 = IsButton1Pressed(state);
	bool bPressingButton2 = IsButton2Pressed(state);

	// This is the first frame the player pressed jump
	if (!bIsInAir && IsButton2Pressed(state) && !bButtonPreviouslyPressed[0])
	{		
		Mix_PlayChannel(CHANNEL_JUMP, JumpSound, 0);
		bIsJumping = true;
		bFirstFrameJump = true;

		JumpInitialVelocityX = fabs(VelocityX);

		if (JumpInitialVelocityX <= VERY_FAST_VELOCITY_X)
		{
			VelocityY = -16.85;
		}
		else
		{
			VelocityY = -20.25;
		}
	}

	if (!IsButton2Pressed(state))
	{
		bIsJumping = false;
	}

	// Start the run release grace period
	if (bButtonPreviouslyPressed[1] && !bPressingButton1)
	{
		RunReleaseCount = 10;
	}

	// Shoot fireball!
	if (!bButtonPreviouslyPressed[1] && bPressingButton1 && PowerUpState == POWER_FLOWER)
	{
		ShootFire();
	}

	if (bPressingButton2)
	{
		bButtonPreviouslyPressed[0] = true;
	}
	else
	{
		bButtonPreviouslyPressed[0] = false;
	}

	if (bPressingButton1)
	{
		bButtonPreviouslyPressed[1] = true;
	}
	else
	{
		bButtonPreviouslyPressed[1] = false;
	}
}

void PlayerSprite::TickAnimation(double DeltaTime)
{
	Sprite::TickAnimation(DeltaTime);

	if (bGrowing && AnimData.bFinished)
	{		
		CollisionRect.y += CollisionRect.y;
		CollisionRect.h *= 2;
		PosY -= 64;
		bGrowing = false;
		PlayAnimation(ResourceManager::PlayerGoombaTallAnimation);
	}

	if (bShrinking && AnimData.bFinished)
	{
		CollisionRect = { 5, 16, 54, 48 };
		PosY += 64;
		SetWidth(64);
		SetHeight(64);
		bShrinking = false;
		PlayAnimation(ResourceManager::PlayerGoombaAnimation);
	}
}

bool PlayerSprite::IsRightPressed(const Uint8 *state)
{
	return state[SDL_SCANCODE_KP_6] || state[SDL_SCANCODE_RIGHT] || (Joy && SDL_JoystickGetAxis(Joy, 0) > JOYSTICK_DEAD_ZONE) || (SDL_JoystickGetHat(Joy, 0) & SDL_HAT_RIGHT);
}

bool PlayerSprite::IsLeftPressed(const Uint8 *state)
{
	return state[SDL_SCANCODE_KP_4] || state[SDL_SCANCODE_LEFT] || (Joy && SDL_JoystickGetAxis(Joy, 0) <= -JOYSTICK_DEAD_ZONE) || (SDL_JoystickGetHat(Joy, 0) & SDL_HAT_LEFT);
}

bool PlayerSprite::IsUpPressed(const Uint8 *state)
{
	return state[SDL_SCANCODE_KP_8] || state[SDL_SCANCODE_UP] || (Joy && SDL_JoystickGetAxis(Joy, JOY_Y_AXIS) < -JOYSTICK_DEAD_ZONE) || (SDL_JoystickGetHat(Joy, 0) & SDL_HAT_UP);
}

bool PlayerSprite::IsDownPressed(const Uint8 *state)
{
	return state[SDL_SCANCODE_KP_2] || state[SDL_SCANCODE_DOWN] || (Joy && SDL_JoystickGetAxis(Joy, JOY_Y_AXIS) > JOYSTICK_DEAD_ZONE) || (SDL_JoystickGetHat(Joy, 0) & SDL_HAT_DOWN);
}

bool PlayerSprite::IsButton1Pressed(const Uint8 *state)
{
	return state[SDL_SCANCODE_LALT] || (Joy && SDL_JoystickGetButton(Joy, 0));
}

bool PlayerSprite::IsButton2Pressed(const Uint8 *state)
{
	return state[SDL_SCANCODE_SPACE] || state[SDL_SCANCODE_RETURN] || (Joy && SDL_JoystickGetButton(Joy, 1));

}

void PlayerSprite::HandleJump()
{
	bool bGoingForward = Flip == SDL_FLIP_NONE && IsRightPressed(LastKeyboardState) || Flip == SDL_FLIP_HORIZONTAL && IsLeftPressed(LastKeyboardState);	
	bool bGoingBackward = Flip == SDL_FLIP_NONE && IsLeftPressed(LastKeyboardState) || Flip == SDL_FLIP_HORIZONTAL && IsRightPressed(LastKeyboardState);	

	if (bGoingForward)
	{
		if (fabs(VelocityX) < FAST_AIR_SPEED)
		{
			VelocityX += Flip == SDL_FLIP_NONE ? AIR_FORWARD_ACCELERATION : -AIR_FORWARD_ACCELERATION;
		}
		else
		{
			VelocityX += Flip == SDL_FLIP_NONE ? AIR_FORWARD_ACCELERATION_FAST : -AIR_FORWARD_ACCELERATION_FAST;
		}
	}
	else if (bGoingBackward)
	{
		if (fabs(VelocityX) >= FAST_AIR_SPEED)
		{
			VelocityX += Flip == SDL_FLIP_NONE ? -AIR_BACKWARD_ACCELERATION_FAST : AIR_BACKWARD_ACCELERATION_FAST;
		}
		else
		{
			if (fabs(JumpInitialVelocityX) >= FAST_START_JUMP_SPEED)
			{
				VelocityX += Flip == SDL_FLIP_NONE ? -AIR_BACKWARD_ACCELERATION_FAST_START : AIR_BACKWARD_ACCELERATION_FAST_START;
			}
			else
			{
				VelocityX += Flip == SDL_FLIP_NONE ? -AIR_BACKWARD_ACCELERATION_SLOW_START : AIR_BACKWARD_ACCELERATION_SLOW_START;
			}
		}
	}

	if (fabs(JumpInitialVelocityX) < FAST_START_JUMP_SPEED && fabs(VelocityX) > MAX_WALK_SPEED)
	{
		VelocityX = VelocityX > 0 ? MAX_WALK_SPEED : -MAX_WALK_SPEED;
	}
	else if (fabs(JumpInitialVelocityX) >= FAST_START_JUMP_SPEED && fabs(VelocityX) > MAX_RUN_SPEED)
	{
		VelocityX = VelocityX > 0 ? MAX_RUN_SPEED : -MAX_RUN_SPEED;
	}

	// If we're falling, we're not jumping!
	if (VelocityY > 0)
	{
		bIsJumping = false;
	}
}

PlayerSprite::PlayerSprite()
{
	NumRedCoins = 0;
	bDrawHUD = true;
	bExitedLevel = false;
	EndOfLevelCountdown = 0;
	bExitingLevel = false;
	JumpOffPoleCountDown = 0;	
	bRidingFlagPole = false;
	EjectionDirection = DIRECTION_NONE;
	StompCount = 0;
	TripLevel = 0;
	bSpriteVisible = true;
	bDead = false;
	Score = 0;
	Lives = 3;
	DyingCount = 0;
	InvincibleCount = 0;
	StarCountdown = 0;
	Coins = 0;
	bWarping = false;
	NumFireBalls = 0;
	RunReleaseCount = 0;
	PowerUpState = POWER_NONE;
	LastKeyboardState = SDL_GetKeyboardState(NULL);
	bFirstFrameJump = false;
	bIsInAir = false;
	JumpInitialVelocityX = 0;
	bIsRunning = false;
	bIsJumping = false;
	bGrowing = false;
	bShrinking = false;
	bDucking = false;
	StartJumpVelocity = 0;	
	CollisionRenderColor.r = 0;
	CollisionRenderColor.g = 255;
	CollisionRenderColor.b = 0;
	//CollisionRect = { 8, 16, 54, 48 };
	//CollisionRect = { 5, 8, 54, 56 }; USE THIS ONE

	CollisionRect = { 5, 16, 54, 48 };
}

void PlayerSprite::StopJumping()
{
	bIsJumping = false;
	VelocityY = BASE_FALL_VELOCITY;
	JumpInitialVelocityX = VelocityX;
}

bool PlayerSprite::HandleVerticalMovement()
{
	int NumPixelsToMove = floor(fabs(VelocityY));
	SDL_Rect NewRect = GetScreenSpaceCollisionRect(); // { PosX - floor(TheMap->GetScrollX()), PosY - floor(TheMap->GetScrollY()), 64, 64 };
	bool bCollided = false;
	bool bIgnoreBrickOntheWayUp = false;
	SDL_Rect MapSpaceCollisionRect = GetMapSpaceCollisionRect();

	//// If we're half way off the pixel
	//if (MapSpaceCollisionRect.x % 64 >= 32)
	//{
	//	NewRect.x += MapSpaceCollisionRect.x % 64;
	//}
	//else
	//{
	//	NewRect.x -= MapSpaceCollisionRect.x % 64;
	//}
	
	for (int y = 0; y < NumPixelsToMove; y++)
	{
		// Check one pixel above us
		NewRect.y += VelocityY > 0 ? 1 : -1;
		vector<TileInfo> HitTiles;
		// Did we bump into a brick going up?
		bCollided = TheMap->CheckCollision(NewRect, HitTiles);

		// If we have 4 tiles returned that means there is a chance we hit a block on the edge, in which case we need to clear 
		// collided flag to let the user jump around the block.

		if ((HitTiles.size() == 4 || HitTiles.size() == 6) && bCollided)
		{
			TileInfo SingleHitTile({ 0, 0, }, TILE_NONE, -1);

			int BottomLeftTileType = TheMap->GetMetaTileType(HitTiles[0].Location.x, HitTiles[0].Location.y);
			int BottomRightTileType = TheMap->GetMetaTileType(HitTiles[1].Location.x, HitTiles[1].Location.y);
			bool bBottomLeftIsCollidable = TheMap->IsCollidableTile(BottomLeftTileType, HitTiles[0].Location.x, HitTiles[0].Location.y);
			bool bBottomRightIsCollidable = TheMap->IsCollidableTile(BottomRightTileType, HitTiles[1].Location.x, HitTiles[1].Location.y);
					
			//SingleHitTile = bBottomLeftIsCollidable ? HitTiles[0] : HitTiles[1];				
			// Closer to the left tile
  			if (MapSpaceCollisionRect.x % 64 <= 32)
			{
				SingleHitTile = HitTiles[0];
			}
			else
			{					
				SingleHitTile = HitTiles[1];
			}

			// If both tiles we hit are collidable, choose to hit the one we are covering more
   			if (bBottomLeftIsCollidable && bBottomRightIsCollidable)
			{
				HitTiles.clear();
				HitTiles.push_back(SingleHitTile);
			}
			else if (bBottomLeftIsCollidable != bBottomRightIsCollidable)
			{
				// Slide up along the left side of this tile
  				if (!bBottomLeftIsCollidable && (MapSpaceCollisionRect.x + MapSpaceCollisionRect.w) % 64 <= 12)
				{
					HitTiles.clear();
					bCollided = false;
					PosX--;

					if (GetMapSpaceCollisionRect().x < TheMap->GetScrollX())
					{
						bIgnoreBrickOntheWayUp = true;
					}
				}
				// slide up along side the right side of this tile
				else if (!bBottomRightIsCollidable && MapSpaceCollisionRect.x % 64 >= 52)
				{
					HitTiles.clear();
					bCollided = false;
					PosX++;
				}
			}						
		}


		if (GetMapSpaceCollisionRect().x < TheMap->GetScrollX() && VelocityY < 0)
		{
			bCollided = false;
		}

		if (VelocityY > 100)
		{
			VelocityY = 100;
		}

		for (int i = 0; i < HitTiles.size(); i++)
		{									
			if (HitTiles[i].MetaTileType == TILE_BREAK_ON_TOUCH)
			{
				TheMap->HandleCollision(HitTiles[i].Location.x, HitTiles[i].Location.y, PowerUpState != POWER_NONE);
			}
			// Check to see if we bumped any special blocks
			if (VelocityY < 0)
			{								
				if (TheMap->IsHiddenBlockTile(HitTiles[i].MetaTileType))
				{
					// If this tile is above us, then get out of here			
					if (HitTiles[i].Location.y * 64 + 64 > MapSpaceCollisionRect.y)
					{
						continue;
					}
					
 					TheMap->HandleCollision(HitTiles[i].Location.x, HitTiles[i].Location.y, PowerUpState != POWER_NONE);
					// Move player out of the block tile
					if (!bCollided)
					{
						PosY += 1;
					}

					bCollided = true;
					break;
				}
				else if (TheMap->IsBreakableBlockTile(HitTiles[i].MetaTileType))
				{					
					TheMap->HandleCollision(HitTiles[i].Location.x, HitTiles[i].Location.y, PowerUpState != POWER_NONE);
					// Move player out of the block tile
					//if (!bCollided)
					{
						PosY += 1;						
					}					
					bCollided = true;
					break;
				}
				else if (bCollided)
				{
					Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
				}
			}
		}

		if (bCollided)
		{			
			StopJumping();
		}
		else
		{
			PosY += VelocityY > 0 ? 1 : -1;			
		}
	}

	return bCollided;
}

void PlayerSprite::SetPowerUpState(ePowerUpState NewPowerUpState)
{
	PowerUpState = NewPowerUpState;
}

void PlayerSprite::AddCoins(int Amount)
{
	Coins += Amount;	

	if (Coins >= 100)
	{
		//AddLife();
		Coins = Coins % 100;
	}
	Mix_PlayChannel(CHANNEL_COIN, CoinSound, 0);
}

void PlayerSprite::AddRedCoins(int Amount, int TileX, int TileY)
{
	NumRedCoins += Amount;	

	TheGame->AddGatheredRedCoinLocation({ TileX, TileY });
	Mix_PlayChannel(CHANNEL_RED_COIN, RedCoinSound, 0);
}

void PlayerSprite::AddScore(int Amount, int PointSpriteX, int PointSpriteY, bool bIsOneUp)
{
	if (PointSpriteX != -1 && PointSpriteY != -1)
	{
		if (bIsOneUp)
		{
			SimpleSprites.push_back(new PointSprite(PointSpriteX, PointSpriteY, "1UP"));
			Lives++;
			Mix_PlayChannel(CHANNEL_ONE_UP, OneUpSound, 0);
		}
		else
		{
			SimpleSprites.push_back(new PointSprite(PointSpriteX, PointSpriteY, Amount));
		}
	}
	Score += Amount;
}

void PlayerSprite::SetColorModForAllTextures(SDL_Color ColorMod)
{
	SDL_SetTextureColorMod(GResourceManager->PlayerGoombaTexture->Texture, ColorMod.r, ColorMod.g, ColorMod.b);
	SDL_SetTextureColorMod(GResourceManager->PlayerGoombaTallTexture->Texture, ColorMod.r, ColorMod.g, ColorMod.b);
	SDL_SetTextureColorMod(GResourceManager->GoombaGrowTexture->Texture, ColorMod.r, ColorMod.g, ColorMod.b);
	SDL_SetTextureColorMod(GResourceManager->GoombaDuckTexture->Texture, ColorMod.r, ColorMod.g, ColorMod.b);	

	SDL_SetTextureColorMod(GResourceManager->PlayerGoombaTexture->Texture2, ColorMod.r, ColorMod.g, ColorMod.b);
	SDL_SetTextureColorMod(GResourceManager->PlayerGoombaTallTexture->Texture2, ColorMod.r, ColorMod.g, ColorMod.b);
	SDL_SetTextureColorMod(GResourceManager->GoombaGrowTexture->Texture2, ColorMod.r, ColorMod.g, ColorMod.b);
	SDL_SetTextureColorMod(GResourceManager->GoombaDuckTexture->Texture2, ColorMod.r, ColorMod.g, ColorMod.b);
}

void PlayerSprite::Stomp(Sprite* Other)
{
	bool bIsOneUp = false;
	Other->GetStomped();
	
	if (StompCount == 9)
	{
		bIsOneUp = true;
	}
	
	AddScore(StompPoints[StompCount], Other->GetPosX(), Other->GetPosY(), bIsOneUp);
	IncreaseStompCounter(1);
	Mix_PlayChannel(CHANNEL_STOMP, StompSound, 0);	
	bStompedLastFrame = true;
	//Launch();
}

void PlayerSprite::Launch(double InVelocityY)
{
	if (IsOnGround())
	{		
		PosY-=2;
		Rect.y-=2;
	}

	VelocityY = InVelocityY;
}

void PlayerSprite::IncreaseStompCounter(int Amount)
{
	StompCount++;
	if (StompCount > 9)
	{		
		StompCount = 9;
	}
}

void PlayerSprite::Kick(Sprite* Other)
{
	Other->GetKicked(this);

	// TODO: Get kick value from other sprite
	//AddScore(100, Other->GetPosX(), Other->GetPosY());
	Mix_PlayChannel(CHANNEL_KICK, KickSound, 0); 
}

bool PlayerSprite::IsChangingSize()
{
	return bGrowing || bShrinking || DyingCount;
}

//SDL_Rect PlayerSprite::GetScreenSpaceCustomRect()
//{
//	return{ 0, 0, 64, 64 };
//}

void PlayerSprite::Duck()
{
	bDucking = true;

	if (PowerUpState >= POWER_BIG)
	{
		CollisionRect = { 5, 74, 54, 54 };
		PlayAnimation(GResourceManager->GoombaDuckAnimation);
	}
}

void PlayerSprite::StandUp()
{
	bDucking = false;
	if (PowerUpState >= POWER_BIG)
	{
		PlayAnimation(GResourceManager->PlayerGoombaTallAnimation);
		CollisionRect = { 5, 32, 54, 96 };
	}
	else
	{
		PlayAnimation(GResourceManager->PlayerGoombaAnimation);
	}
}

void PlayerSprite::ShootFire()
{
	if (NumFireBalls < 2 && !bDucking)
	{
		int SpawnX = PosX / 64 * 64 + 32;
		int SpawnY = PosY / 64 * 64 + 4;
		double Velocity = 16;

		if (Flip & SDL_FLIP_HORIZONTAL)
		{
			Velocity = -16;
			SpawnX = PosX / 64 * 64 + 16;
		}
		FireBallSprite* FireSprite = new FireBallSprite(SpawnX, SpawnY, Velocity);
		SimpleSprites.push_back(FireSprite);
		NumFireBalls++;

		Mix_PlayChannel(CHANNEL_FIRE_BALL, FireBallSound, 0);
	}
}

void PlayerSprite::OnFireBallDestroy()
{
	if (NumFireBalls > 0)
	{
		NumFireBalls--;
	}
}

void PlayerSprite::StartWarpSequence(WarpEntrance Entrance, WarpExit Exit)
{	
	if (Exit.WarpType == WARP_CONTROL)
	{
		// TODO:
		// ApplyControlSettings();
	}
	else
	{
		SetAnimationPlayRate(0);
		VelocityX = 0;
		VelocityY = 0;
		bWarping = true;
		WarpSeq.FrameCount = 0;
		WarpSeq.Entrance = Entrance;
		WarpSeq.Exit = Exit;
		WarpSeq.bWarpComplete = false;
		WarpSeq.bWarpExitComplete = false;
		Mix_PlayChannel(CHANNEL_PIPE, PipeSound, 0);
		TheMap->StopSAutoScroll();
	}
}

bool PlayerSprite::IsWarping()
{
	return bWarping;
}

void PlayerSprite::CheckCollisionWithSprites()
{
	if (bWarping || IsDying())
	{
		return;
	}
	Sprite::CheckCollisionWithSprites();

	CheckWarpCollision();
	CheckControlCollision();
}

void PlayerSprite::CheckWarpCollision()
{
	vector<WarpInfo> Warps = TheMap->GetWarps();

	bool bIsOnGround = IsOnGround();

	//if (Rect.y % 64)
	//{
	//	bIsOnGround = false;
	//}

	for (int i = 0; i < Warps.size(); i++)
	{
		for (int j = 0; j < Warps[i].Entrances.size(); j++)
		{
			SDL_Rect WarpCollision = { Warps[i].Entrances[j].PosX, Warps[i].Entrances[j].PosY, 64, 64 };
			SDL_Rect ResultRect;

			SDL_Rect CollisionRect = GetMapSpaceCollisionRect();

			// Make the warp a little smaller so we can't hang off the edge when going down pipes
			SDL_Rect VerticalWarpCollision = { Warps[i].Entrances[j].PosX + 32, Warps[i].Entrances[j].PosY, 0, 64 };

			// Down pipe
			if (Warps[i].Entrances[j].WarpType == WARP_PIPE_DOWN && IsDownPressed(LastKeyboardState) && CollisionRect.x + CollisionRect.w >= VerticalWarpCollision.x && CollisionRect.x <= VerticalWarpCollision.x + VerticalWarpCollision.w && CollisionRect.y + CollisionRect.h + 1 == VerticalWarpCollision.y)
			{
				StartWarpSequence(Warps[i].Entrances[j], Warps[i].Exits[0]);
			}
			// Up pipe
			else if (Warps[i].Entrances[j].WarpType == WARP_PIPE_UP && IsUpPressed(LastKeyboardState) && CollisionRect.x + CollisionRect.w >= VerticalWarpCollision.x && CollisionRect.x <= VerticalWarpCollision.x + VerticalWarpCollision.w && CollisionRect.y - 6 <= VerticalWarpCollision.y + VerticalWarpCollision.h)
			{
				StartWarpSequence(Warps[i].Entrances[j], Warps[i].Exits[0]);
			}
			// Right pipe
			// TODO: Add additional x check 
			else if (bIsOnGround && (Warps[i].Entrances[j].WarpType == WARP_PIPE_RIGHT || Warps[i].Entrances[j].WarpType == WARP_PIPE_RIGHT_SECOND_WINDOW) && IsRightPressed(LastKeyboardState) && CollisionRect.x <= WarpCollision.x + WarpCollision.w && CollisionRect.x + CollisionRect.w + 1 >= WarpCollision.x && CollisionRect.y + CollisionRect.h >= WarpCollision.y && CollisionRect.y <= WarpCollision.y + WarpCollision.h)
			{
				StartWarpSequence(Warps[i].Entrances[j], Warps[i].Exits[0]);
			}
			// Left pipe
			else if (bIsOnGround && (Warps[i].Entrances[j].WarpType == WARP_PIPE_LEFT) && IsLeftPressed(LastKeyboardState) && CollisionRect.x > WarpCollision.x && CollisionRect.x - 1 <= WarpCollision.x + WarpCollision.w && CollisionRect.y + CollisionRect.h >= WarpCollision.y && CollisionRect.y <= WarpCollision.y + WarpCollision.h)
			{
				StartWarpSequence(Warps[i].Entrances[j], Warps[i].Exits[0]);
			}
			else if (Warps[i].Entrances[j].WarpType == WARP_INSTANT)
			{
				// Intersect is not inclusive, so widen the collision box
				WarpCollision = { Warps[i].Entrances[j].PosX, Warps[i].Entrances[j].PosY, 64, 64 };
				WarpCollision.x -= 1;
				WarpCollision.w += 2;
				WarpCollision.y -= 1;
				WarpCollision.h += 2;

				if (SDL_IntersectRect(&WarpCollision, &CollisionRect, &ResultRect))
				{
					// TODO: Select the warp exit to use
					StartWarpSequence(Warps[i].Entrances[j], Warps[i].Exits[0]);
				}
			}
			else if (GWindow2 != NULL)
			{
				if (bIsOnGround && (Warps[i].Entrances[j].WarpType == WARP_PIPE_LEFT_FIRST_WINDOW) && IsLeftPressed(LastKeyboardState) && CollisionRect.x > WarpCollision.x && CollisionRect.x - 1 <= WarpCollision.x + WarpCollision.w && CollisionRect.y + CollisionRect.h >= WarpCollision.y && CollisionRect.y <= WarpCollision.y + WarpCollision.h)
				{
					int Window1X;
					int Window1Y;
					int Window2X;
					int Window2Y;
					int Window1Width;
					int Window1Height;
					int Window2Width;
					int Window2Height;

					SDL_GetWindowPosition(GWindow, &Window1X, &Window1Y);
					SDL_GetWindowPosition(GWindow2, &Window2X, &Window2Y);
					SDL_GetWindowSize(GWindow, &Window1Width, &Window1Height);				
					SDL_GetWindowSize(GWindow2, &Window2Width, &Window2Height);

					if (Window2X >= Window1X + Window1Width - 64 && Window2X <= Window1X + Window1Width + 16)
					{
						int ExitIndex = -1;

						// Connected to the top pipe
						if (Window2Y >= Window1Y - 16 && Window2Y <= Window1Y + 16)
						{
							ExitIndex = 0;
						}
						// Connected to bottom pipe
						else if (Window2Y + Window2Height >= Window1Y + Window1Height - 16 && Window2Y + Window2Height <= Window1Y + Window1Height + 16)
						{
							ExitIndex = 1;
						}

						if (ExitIndex > -1)
						{
							StartWarpSequence(Warps[i].Entrances[j], Warps[i].Exits[ExitIndex]);
						}
					}
				}
			}
		}
	}
}

void PlayerSprite::CheckControlCollision()
{
	if (IsDying())
	{
		return;
	}
	vector<ControlTrigger*> Controls = TheMap->GetControlTiles();
	SDL_Rect CollisionRect = GetMapSpaceCollisionRect();
	SDL_Rect ResultRect;
	
	// Intersect is not inclusive, so widen the collision box
	CollisionRect.x -= 1;
	CollisionRect.w += 2;
	CollisionRect.y -= 1;
	CollisionRect.h += 2;

	for (int i = 0; i < Controls.size(); i++)
	{			

		SDL_Rect ControlCollision = { Controls[i]->PosX, Controls[i]->PosY, Controls[i]->Width, Controls[i]->Height };
				
		if (SDL_IntersectRect(&ControlCollision, &CollisionRect, &ResultRect))
		{
			if (!Controls[i]->bTriggered)
			{
				Controls[i]->bTriggered = true;

				TheGame->HandleSpecialEvent(Controls[i]->SpecialEvent);

				if (Controls[i]->SpecialEvent == SPECIAL_EVENT_LOADING_LEVEL)
				{
					bDrawHUD = true;				
				}
				int WarpIndex = TheMap->GetWarpIndex(Controls[i]->WarpID);

				if (WarpIndex != -1)
				{
					TheGame->HandleControl(Controls[i]);
				}
				else
				{
					SDL_Log("WarpID %d not found for control %d", Controls[i]->WarpID, Controls[i]->ID);
				}
			}
		}
		else
		{
			if (Controls[i]->bTriggered && Controls[i]->bResetWhenPlayerLeaves)
			{
				Controls[i]->bTriggered = false;
			}
		}
	}
}

void PlayerSprite::UpdateWarpSequence()
{	
	if (WarpSeq.bWarpComplete && !WarpSeq.bWarpExitComplete)
	{
		UpdateWarpExitSequence();
	}
	else
	{
		SDL_Point DeltaPos = { 0, 0 };

		if (WarpSeq.Entrance.WarpType == WARP_PIPE_UP)
		{
			DeltaPos.y = -2;
		}
		else if (WarpSeq.Entrance.WarpType == WARP_PIPE_DOWN)
		{
			DeltaPos.y = 2;
		}
		else if (WarpSeq.Entrance.WarpType == WARP_PIPE_LEFT || WarpSeq.Entrance.WarpType == WARP_PIPE_LEFT_FIRST_WINDOW)
		{
			DeltaPos.x = -2;
		}
		else if (WarpSeq.Entrance.WarpType == WARP_PIPE_RIGHT || WARP_PIPE_RIGHT_SECOND_WINDOW)
		{
			DeltaPos.x = 2;
		}

		if ((WarpSeq.Entrance.WarpType >= WARP_PIPE_UP && WarpSeq.Entrance.WarpType <= WARP_PIPE_RIGHT) || WarpSeq.Entrance.WarpType == WARP_PIPE_LEFT_FIRST_WINDOW || WarpSeq.Entrance.WarpType == WARP_PIPE_RIGHT_SECOND_WINDOW)
		{
			if (WarpSeq.FrameCount == 0)
			{
				if (WarpSeq.Entrance.WarpType == WARP_PIPE_DOWN && PowerUpState >= POWER_BIG)
				{
					Duck();
				}
				if (DeltaPos.x > 0)
				{
					SetFlip(SDL_FLIP_NONE);
					SetAnimationPlayRate(0.8);
				}
				else if (DeltaPos.x < 0)
				{
					SetFlip(SDL_FLIP_HORIZONTAL);
					SetAnimationPlayRate(0.8);
				}

				// Fast forward a bit
				/*if (WarpSeq.Entrance.WarpType == WARP_PIPE_RIGHT_SECOND_WINDOW)
				{
					WarpSeq.FrameCount = 58;
				}*/
				// TODO: If flag is set on the entrance, force duck
				//Duck();
			}
			if (WarpSeq.FrameCount <= 32)
			{
				PosX += DeltaPos.x;
				PosY += DeltaPos.y;
				
				// Turn right and do animation				
			}
			else if (!WarpSeq.Entrance.bQuickTransition && WarpSeq.FrameCount <= 58)
			{
				if (WarpSeq.Exit.MusicChange)
				{
					Mix_PauseMusic();
				}
				GRenderBlackout = true;
			}
			else if (WarpSeq.FrameCount > 58 || (WarpSeq.Entrance.bQuickTransition && WarpSeq.Entrance.WarpType == WARP_INSTANT))
			{				
				WarpSeq.bWarpComplete = true;
				SetPosition(WarpSeq.Exit.PosX, (WarpSeq.Exit.PosY - 1) - (Rect.h - 64));
				
				if (WarpSeq.Entrance.WarpType == WARP_PIPE_LEFT_FIRST_WINDOW)
				{
					ShowWindow2(false);
				}
			}
		}		
		else if (WarpSeq.Entrance.WarpType == WARP_INSTANT)
		{
			WarpSeq.bWarpComplete = true;
			SetPosition(WarpSeq.Exit.PosX, (WarpSeq.Exit.PosY - 1) - (Rect.h - 64));
		}

		Rect.x = PosX;// -TheMap->GetScrollX();
		Rect.y = PosY;

		WarpSeq.FrameCount++;		

		if (WarpSeq.bWarpComplete)
		{
			if (WarpSeq.Exit.MusicChange)
			{
				Mix_PlayMusic(GetMusicFromID(WarpSeq.Exit.MusicChange), -1);
			}

			// TODO: Have the game do this shit in HandleControl
			BGColor = WarpSeq.Exit.ExitBGColor;
			// We're about to start the warp exit, so set the complete to false
			WarpSeq.FrameCount = 0;			
		}
	}
}

void PlayerSprite::UpdateWarpExitSequence()
{
	if (WarpSeq.FrameCount == 0)
	{
		//SetPosition(WarpSeq.Exit.PosX, (WarpSeq.Exit.PosY - 1) - (Rect.h - 64));
		VelocityX = 0;
		VelocityY = 0;
		TheMap->DoWarp(WarpSeq.Exit);
		GRenderBlackout = false;

		if (PowerUpState >= POWER_BIG)
		{
			PlayAnimation(ResourceManager::PlayerGoombaTallAnimation);
		}
		else
		{
			PlayAnimation(GResourceManager->PlayerGoombaAnimation);
		}

		SetAnimationPlayRate(0);		
	}
	
	if (WarpSeq.Exit.WarpType == WARP_INSTANT)
	{
		WarpSeq.bWarpExitComplete = true;
	}
	else if (WarpSeq.Exit.WarpType == WARP_PIPE_SHOOT_UP)
	{
		if (WarpSeq.FrameCount == 0)
		{
			PosY += -4;
			VelocityY = -64;
			Mix_PlayChannel(CHANNEL_BUMP, FireworkSound, 0);
			TheGame->HandleSpecialEvent(SPECIAL_EVENT_PIPE_SHOOT);
		}
		else
		{
			WarpSeq.bWarpExitComplete = true;
		}
		
	}
	else if (WarpSeq.Exit.WarpType >= WARP_PIPE_UP && WarpSeq.Exit.WarpType <= WARP_PIPE_RIGHT || WarpSeq.Exit.WarpType == WARP_PIPE_RIGHT_SECOND_WINDOW)
	{
		SDL_Point StartDelta = { 0, 0 };
		SDL_Point DeltaPos = { 0, 0 };

		if (WarpSeq.Exit.WarpType == WARP_PIPE_UP)
		{
			DeltaPos.y = -4;
			StartDelta.y = 128;
		}
		else if (WarpSeq.Exit.WarpType == WARP_PIPE_DOWN)
		{
			DeltaPos.y = 4 + Rect.h / 64;
			StartDelta.y = -128;
		}
		else if (WarpSeq.Exit.WarpType == WARP_PIPE_LEFT)
		{
			DeltaPos.x = -2;
			StartDelta.x = 64;
		}
		else if (WarpSeq.Exit.WarpType == WARP_PIPE_RIGHT || WarpSeq.Exit.WarpType == WARP_PIPE_RIGHT_SECOND_WINDOW)
		{
			DeltaPos.x = 2;
			StartDelta.x = -64;
		}		

		if (WarpSeq.FrameCount == 0)
		{
			PosX += StartDelta.x;
			PosY += StartDelta.y;

			if (DeltaPos.x > 0)
			{
				SetFlip(SDL_FLIP_NONE);
				SetAnimationPlayRate(0.8);
			}
			else if (DeltaPos.x < 0)
			{
				SetFlip(SDL_FLIP_HORIZONTAL);
				SetAnimationPlayRate(0.8);
			}

			if (WarpSeq.Entrance.WarpType == WARP_PIPE_RIGHT_SECOND_WINDOW)
			{
				ShowWindow2(true);
			}
		}
		else if (WarpSeq.FrameCount <= 67)
		{
			
		}
		else if (WarpSeq.FrameCount <= 99)
		{
			PosX += DeltaPos.x;
			PosY += DeltaPos.y;
		}
		else
		{						
			WarpSeq.bWarpExitComplete = true;
						
		}
	}
		
	Rect.x = PosX;// -TheMap->GetScrollX();
	Rect.y = PosY;

	WarpSeq.FrameCount++;

	// Account for big man by subtracting off difference on the y

	if (WarpSeq.bWarpExitComplete)
	{				
		VelocityX = 0;

		if (WarpSeq.Exit.WarpType != WARP_PIPE_SHOOT_UP)
		{
			VelocityY = 0;
		}
		bWarping = false;
	}
}

void PlayerSprite::TakeDamage()
{
	if (PowerUpState >= POWER_BIG)
	{
		OnGetSmall();
		SetColorModForAllTextures({ 255, 255, 255, 255 });
	}
	else
	{
		BeginDie();
	}
}

void PlayerSprite::Render(SDL_Renderer* Renderer, int ResourceNum)
{	
	if (bSpriteVisible)
	{
		int TempX = Rect.x;
		int TempY = Rect.y;
			
		if (TripLevel >= 2 && TripLevel < 4)
		{
			Rect.x = oldx;
			Rect.y = oldy;
			SetColorModForAllTextures(BGColor);
			Sprite::Render(Renderer, ResourceNum);
			SetColorModForAllTextures({ 255, 255, 255 });
		}		

		Rect.x = TempX;
		Rect.y = TempY;
		Sprite::Render(Renderer, ResourceNum);
	}
}

void PlayerSprite::UpdateDyingAnimation()
{
	int DyingFrame = PLAYER_DYING_COUNT - DyingCount;
	
	if (DyingFrame <= 16)
	{

	}
	else if (DyingFrame == 17)
	{
		VelocityY = -18;
	}
	else if (DyingFrame > 17)
	{
		VelocityY +=  BASE_FALL_VELOCITY/2;
	}

	PosY += VelocityY;
	Rect.y = PosY;

	DyingCount--;

	if (DyingCount <= 0)
	{
		bDead = true;
		PowerUpState = POWER_NONE;
	}
}

void PlayerSprite::DrawHUD()
{		
	if (!bDrawHUD)
	{
		return;
	}
	// Draw score
	char TempString[10];
	itoa(Score, TempString, 10);
	DrawBitmapText("GARIO", 97, 32, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
	
	int i = 0;
	int NumZeros = 6 - strlen(TempString);
	NumZeros = NumZeros < 0 ? 0 : NumZeros;

	for (i = 0; i < NumZeros; i++)
	{ 
		DrawBitmapText("0", 97 + i * (GlyphSpace * 32), 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
	}
	DrawBitmapText(TempString, 97 + i * (GlyphSpace * 32), 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
		
	// Draw red coins
	// Draw coins	
	SDL_Rect SrcRect = { 0, 0, 64, 64 };
	SDL_Rect DstRect;

	for (i = 0; i < NumRedCoins; i++)
	{
		DstRect = { 350 + i * 32, 30, 32, 32 };

		SDL_RenderCopy(GRenderer, GResourceManager->RedCoinEffectTexture->Texture, &SrcRect, &DstRect);
	}

	// Draw coins	
	SrcRect = { 0, 0, 64, 64 };
	DstRect = { 350, 60, 32, 32 };

	SDL_RenderCopy(GRenderer, GResourceManager->CoinEffectTexture->Texture, &SrcRect, &DstRect);
	itoa(Coins, TempString, 10);
	DrawBitmapText("X", 389, 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
	NumZeros = 2 - strlen(TempString);
	NumZeros = NumZeros < 0 ? 0 : NumZeros;

	for (i = 0; i < NumZeros; i++)
	{
		DrawBitmapText("0", 420 + i * (GlyphSpace * 32), 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
	}
	DrawBitmapText(TempString, 420 + i * (GlyphSpace * 32), 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);

	// Draw the world string
	DrawBitmapText("WORLD", 578, 32, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
	DrawBitmapText(TheGame->GetWorldName(), 602, 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);

	// Draw the time string	
	itoa(TheMap->GetSecondsLeft(), TempString, 10);
	DrawBitmapText("TIME", 827, 32, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
	NumZeros = 3 - strlen(TempString);
	NumZeros = NumZeros < 0 ? 0 : NumZeros;

	if (TheMap->LevelPlaying())
	{
		for (i = 0; i < NumZeros; i++)
		{
			DrawBitmapText("0", 845 + i * (GlyphSpace * 32), 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
		}
		DrawBitmapText(TempString, 845 + i * (GlyphSpace * 32), 60, 32, 32, GRenderer, FontShadowedWhite, GlyphSpace, 1.25, false);
	}
	
}

void PlayerSprite::BeginLevel()
{			
	SetAnimationPlayRate(1);
	bStompedLastFrame = false;
	bFrozen = false;
	NumRedCoins = TheGame->GetNumberOfRedCoinsFound();
	RenderLayer = RENDER_LAYER_TOP;	
	bDrawHUD = true;
	bExitedLevel = false;
	EndOfLevelCountdown = 0;
	JumpOffPoleCountDown = 0;
	VelocityY = 0;
	VelocityX = 0;
	bExitingLevel = false;
	bRidingFlagPole = false;
	EjectionDirection = DIRECTION_NONE;
	StompCount = 0;
	bRenderCollision = false;
	bWorldSmear = false;
	TripLevel = 0;
	SetFlip(SDL_FLIP_NONE);
	bSpriteVisible = true;
	bDead = false;
	DyingCount = 0;
	InvincibleCount = 0;
	StarCountdown = 0;
	bWarping = false;
	NumFireBalls = 0;
	RunReleaseCount = 0;
	LastKeyboardState = SDL_GetKeyboardState(NULL);
	bFirstFrameJump = false;
	bIsInAir = false;
	JumpInitialVelocityX = 0;
	bIsRunning = false;
	bIsJumping = false;
	bGrowing = false;
	bShrinking = false;
	bDucking = false;
	StartJumpVelocity = 0;
	CollisionRenderColor.r = 0;
	CollisionRenderColor.g = 255;
	CollisionRenderColor.b = 0;
	//CollisionRect = { 8, 16, 54, 48 };
	//CollisionRect = { 5, 8, 54, 56 }; USE THIS ONE


	if (PowerUpState >= POWER_BIG)
	{		
		CollisionRect = { 5, 32, 54, 96 };
		SetWidth(64);
		SetHeight(128);		
		PlayAnimation(GResourceManager->PlayerGoombaTallAnimation, true);
	}
	else
	{
		CollisionRect = { 5, 16, 54, 48 };		
		SetWidth(64);
		SetHeight(64);	
		PlayAnimation(ResourceManager::PlayerGoombaAnimation);
	}

	if (PowerUpState < POWER_FLOWER)
	{
		SetColorModForAllTextures({ 255, 255, 255 });
	}
	else
	{
		SetColorModForAllTextures({ 255, 128, 128 });
	}

	if (TheGame->GetCurrentLevelIndex() == 5)
	{
		RenderLayer = RENDER_LAYER_BEHIND_FG;
		bDrawHUD = false;
	}
}

void PlayerSprite::EndLevel()
{
	NumRedCoins = 0;
}

bool PlayerSprite::IsDead()
{
	return bDead;
}

bool PlayerSprite::IsDying()
{
	return DyingCount;
}

void PlayerSprite::BeginDie()
{
	TheMap->OnPlayerDie();
	Mix_PlayMusic(DieMusic, 0);
	StopAnimation();
	SetTexture(GResourceManager->PlayerGoombaDeadTexture->Texture);		
	VelocityX = 0;
	VelocityY = 0;
	DyingCount = PLAYER_DYING_COUNT;	
	Lives--;
}

void PlayerSprite::GrabFlagPole(FlagPoleSprite* FlagPole)
{
	VelocityX = 0;
	bRidingFlagPole = true;
	VelocityY = 8;
	PosX = FlagPole->GetPosX() - 16;	
	Rect.x = PosX;	
	ExitLevelX = FlagPole->GetPosX() + 6 * 64;
	SetFlip(SDL_FLIP_NONE);
	RenderLayer = RENDER_LAYER_TOP;
	TheGame->OnGrabFlagPole(FlagPole->IsSecretExit());
}

void PlayerSprite::UpdateFlagPoleAnimation()
{
	
	if (!IsOnGround())
	{
		for (int i = 0; i < 8; i++)
		{
			PosY++;

			if (IsOnGround())
			{
				PosY--;
				break;
			}
		}
	}	
}

void PlayerSprite::OnFlagFinished()
{
	SetFlip(SDL_FLIP_HORIZONTAL);
	PosX += 40;
	JumpOffPoleCountDown = 24;
	Rect.x = PosX;	
	VelocityX = WALKING_ACCELERATION;
	bRidingFlagPole = false;	
}

bool PlayerSprite::HasExitedLevel()
{
	return bExitedLevel;
}
