#include "../inc/Sprite.h"
#include "../inc/TMXMap.h"
#include "../inc/ItemSprite.h"
#include "../inc/SpriteList.h"

Sprite::Sprite() :
	Sprite(NULL)
{	
	bForceDrawWhenNotInWindow = true;
	bDeleteWhenNotVisible = true;
}

Sprite::Sprite(SDL_Texture *InTexture)
{
	bForceDrawWhenNotInWindow = true;
	RenderLayer = RENDER_LAYER_TOP;
	bDrawScreenSpace = false;
	bDeleteWhenNotVisible = true;
	CollisionRenderColor.a = 128;
	CollisionRenderColor.r = 255;
	CollisionRenderColor.g = 255;
	CollisionRenderColor.b = 255;

	Scale = 1;
	CountDown = -1;
	Flip = SDL_FLIP_NONE;
	Rect.x = 0;
	Rect.y = 0;
	Rect.w = 0;
	Rect.h = 0;

	PosX = 0;
	PosY = 0;
	
	VelocityX = 0;
	VelocityY = 0;

	bPendingDelete = false;
	bDeleteWhenNotVisible = true;

	MovingFlags = MOVING_NONE;
	CollisionRect = { 0, 0, Rect.w, Rect.h };

	SetTexture(InTexture);
}

void Sprite::SetFlip(SDL_RendererFlip NewFlip)
{
	Flip = NewFlip;
}

void Sprite::Tick(double DeltaTime)
{
	HandleInput(DeltaTime);	
	TickAnimation(DeltaTime);

	if (bDeleteWhenNotVisible && !bDrawScreenSpace && !InMapWindow({ SPAWN_DISTANCE * 2, SPAWN_DISTANCE * 2}))
	{
		bPendingDelete = true;
	}
	
	/*static int Count = 0;
	Count++;
	if (Count % 32 == 0)
	{
		Flip = (SDL_RendererFlip(SDL_FLIP_NONE + rand() % 4));
	}*/
	CheckCollisionWithSprites();
}

void Sprite::TickAnimation(double DeltaTime)
{
	if (AnimData.Anim)
	{
		if (!AnimData.Anim->UseAbsoluteFrameCount())
		{
			AnimData.CountDown -= DeltaTime * AnimData.PlayRate;
		}
		else
		{
			AnimData.CountDown -= 1 * AnimData.PlayRate;
		}

		if (AnimData.CountDown <= 0)
		{
			AnimData.CurrentFrameIndex++;
			if (AnimData.CurrentFrameIndex >= AnimData.Anim->GetFrameCount())
			{
				if (AnimData.bLoop)
				{
					AnimData.CurrentFrameIndex = 0;					
				}				
				else
				{
					AnimData.bFinished = true;

					//if (bDeleteAfterAnimation)
					{
						bPendingDelete = true;
					}
					AnimData.CurrentFrameIndex = AnimData.Anim->GetFrameCount() - 1;
				}
			}
			UpdateAnimationData();
		}
	}
}

void Sprite::HandleInput(double DeltaTime)
{

}

SDL_Rect Sprite::GetScreenSpaceCollisionRect()
{
	SDL_Rect CollisionToUse = CollisionRect;

	if (Flip == SDL_FLIP_HORIZONTAL)
	{
		CollisionToUse.x = Rect.w - (CollisionRect.x + CollisionRect.w);
	}
	CollisionToUse.x += PosX - floor(TheMap->GetScrollX());
	CollisionToUse.y += PosY - floor(TheMap->GetScrollY());
	return CollisionToUse;
}

SDL_Rect Sprite::GetScreenSpaceCustomRect()
{
	return GetScreenSpaceCollisionRect();
}

SDL_Rect Sprite::GetMapSpaceCollisionRect()
{
	SDL_Rect CollisionToUse = CollisionRect;

	if (Flip == SDL_FLIP_HORIZONTAL)
	{
		CollisionToUse.x = 64 - (CollisionRect.x + CollisionRect.w);
	}
	CollisionToUse.x += Rect.x;
	CollisionToUse.y += Rect.y;
	return CollisionToUse;
}

void Sprite::UpdateAnimationData()
{
	Frame *CurFrame = AnimData.Anim->GetFrame(AnimData.CurrentFrameIndex);
	if (CurFrame)
	{
		AnimData.PlayRate = 1;
		AnimData.CountDown = CurFrame->GetFrameTime();
		Texture = CurFrame->GetTexture();
		Texture2 = CurFrame->GetTexture(2);
	}
}

void Sprite::SetTexture(SDL_Texture* InTexture)
{
	Texture = InTexture;

	if (Texture != NULL)
	{
		Uint32 Format;
		int Access;

		SDL_QueryTexture(Texture, &Format, &Access, &Rect.w, &Rect.h);
	}
}

void Sprite::SetPosition(double NewX, double NewY)
{
	PosX = NewX;
	PosY = NewY;
	Rect.x = PosX;
	Rect.y = PosY;
}

void Sprite::SetWidth(int NewWidth)
{
	Rect.w = NewWidth;	
}

void Sprite::SetHeight(int NewHeight)
{
	Rect.h = NewHeight;	
}

void Sprite::Render(SDL_Renderer* Renderer, int ResourceNum)
{
	if (!bDrawScreenSpace && !InMapWindow() && !bForceDrawWhenNotInWindow)
	{
		return;
	}

	SDL_Rect SrcRect = { 0, 0, Rect.w, Rect.h};
	if (AnimData.Anim)
	{
		Frame *CurFrame = AnimData.Anim->GetFrame(AnimData.CurrentFrameIndex);
		if (CurFrame)
		{
			SDL_Rect DstRect = Rect;			

			if (!bDrawScreenSpace)
			{
				DstRect.x -= floor(TheMap->GetScrollX());
				DstRect.y -= floor(TheMap->GetScrollY());
			}

			SrcRect = CurFrame->GetSrcRect();

			/*SDL_Rect ShadowRect;
			SDL_Rect ShadowSrcRect = { 0, 0, 68, 51 };
			ShadowRect.x = Rect.x - (Rect.w * 0.6) * 0.5 + 40;
			ShadowRect.y = Rect.y + Rect.h - Rect.h * 0.15 + 6;
			ShadowRect.w = (double)Rect.w * 0.6;
			ShadowRect.h = (double)Rect.h * 0.15;

			SDL_RenderCopyEx(Renderer, ResourceManager::ShadowTexture->Texture, &ShadowSrcRect, &ShadowRect, 0, NULL, SDL_FLIP_NONE);*/

			DstRect.x *= RenderScale;
			DstRect.y *= RenderScale;
			DstRect.w *= RenderScale;
			DstRect.h *= RenderScale;
			
			if (ResourceNum == 2)
			{
				SDL_RenderCopyEx(Renderer, Texture2, &SrcRect, &DstRect, 0, NULL, Flip);
			}
			else
			{
				SDL_RenderCopyEx(Renderer, Texture, &SrcRect, &DstRect, 0, NULL, Flip);
			}
			
		}		
	}
	else if (Texture)
	{
		SDL_Rect DstRect = Rect;		

		if (!bDrawScreenSpace)
		{
			DstRect.x -= TheMap->GetScrollX();
			DstRect.y -= TheMap->GetScrollY();
		}
		
		DstRect.w *= Scale;
		DstRect.h *= Scale;

		DstRect.x *= RenderScale;
		DstRect.y *= RenderScale;
		DstRect.w *= RenderScale;
		DstRect.h *= RenderScale;
		
		if (ResourceNum == 2)
		{
			SDL_RenderCopyEx(Renderer, Texture2, &SrcRect, &DstRect, 0, NULL, Flip);
		}
		else
		{
			SDL_RenderCopyEx(Renderer, Texture, &SrcRect, &DstRect, 0, NULL, Flip);
		}
	}

	if (bRenderCollision)
	{
		RenderCollision(Renderer);
		TheMap->CheckCollision(GetScreenSpaceCollisionRect());
	}
}

void Sprite::RenderCollision(SDL_Renderer *Renderer)
{
	SDL_Rect ScreenSpaceCollision = GetScreenSpaceCollisionRect();

	ScreenSpaceCollision.x *= RenderScale;
	ScreenSpaceCollision.y *= RenderScale;
	ScreenSpaceCollision.w *= RenderScale;
	ScreenSpaceCollision.h *= RenderScale;

	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Renderer, CollisionRenderColor.r, CollisionRenderColor.g, CollisionRenderColor.b, 128);

	SDL_RenderFillRect(Renderer, &ScreenSpaceCollision);


	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
}

void Sprite::PlayAnimation(AnimationResource *Anim, bool bLoop)
{
	AnimData.Anim = Anim;
	if (Anim)
	{
		AnimData.bFinished = false;
		AnimData.Anim = Anim;
		AnimData.CurrentFrameIndex = 0;
		AnimData.bLoop = bLoop;
		UpdateAnimationData();
	}
}

void Sprite::SetAnimationPlayRate(double Rate)
{
	AnimData.PlayRate = Rate;

	if (Rate == 0)
	{
		AnimData.CurrentFrameIndex = 0;
	}
}

void Sprite::StopAnimation()
{
	AnimData.Anim = NULL;
}

bool Sprite::GetPendingDelete()
{
	return bPendingDelete;
}

void Sprite::Delete()
{
	bPendingDelete = true;
}


void Sprite::SetMovingFlags(eMovingFlags NewFlags)
{
	MovingFlags = NewFlags;
}

int Sprite::GetMovingFlags()
{
	return MovingFlags;
}

bool Sprite::InMapWindow(SDL_Point Offset)
{
	SDL_Rect MapWindow = TheMap->GetVisibleWindow();

	// To the right of the window
	return (PosX + Rect.w >= MapWindow.x - Offset.x && PosX <= MapWindow.x + MapWindow.w + Offset.x &&
		PosY + Rect.h >= MapWindow.y - Offset.y && PosY <= MapWindow.y + MapWindow.h + Offset.y);
}

void Sprite::GetStomped()
{

}

void Sprite::GetKicked(PlayerSprite* Player)
{

}

bool Sprite::IsStompable()
{
	return false;
}

bool Sprite::IsKickable()
{
	return false;
}

void Sprite::GetBricked(int TileX, int TileY)
{

}

void Sprite::CheckCollisionWithSprites()
{
	// No interactions if the player is dying
	if (ThePlayer->IsDying())
	{
		return;
	}

	for (int i = 0; i < EnemySprites.size(); i++)
	{
		EnemySprite* Enemy = EnemySprites[i];

		if (this == Enemy)
		{
			continue;
		}

		SDL_Rect EnemyCollision = Enemy->GetScreenSpaceCollisionRect();
		SDL_Rect ResultRect;

		SDL_Rect CollisionRect = GetScreenSpaceCollisionRect();

		// Intersect is not inclusive, so widen the collision box
		EnemyCollision.x -= 1;
		EnemyCollision.w += 2;
		EnemyCollision.y -= 1;
		EnemyCollision.h += 2;

		if (Enemy->IsInteractable() &&  SDL_IntersectRect(&EnemyCollision, &CollisionRect, &ResultRect))
		{
			bool bContinueInteractingWithEnemies = Interact(Enemy);

			// Can only interact with one thing a frame to prevent things like dying from a goomba
			// while stomping another			
			if (!bContinueInteractingWithEnemies)
			{
				break;
			}
		}
	}	

	for (int i = 0; i < ItemSprites.size(); i++)
	{
		ItemSprite* Item = ItemSprites[i];

		if (this == Item)
		{
			continue;
		}

		SDL_Rect ItemCollision = Item->GetScreenSpaceCollisionRect();
		SDL_Rect ResultRect;

		SDL_Rect CollisionRect = GetScreenSpaceCollisionRect();

		// Intersect is not inclusive, so widen the collision box
		ItemCollision.x -= 1;
		ItemCollision.w += 2;
		ItemCollision.y -= 1;
		ItemCollision.h += 2;

		if (SDL_IntersectRect(&ItemCollision, &CollisionRect, &ResultRect))
		{
			Interact(Item);

			// Can only interact with one thing a frame
			break;
		}
	}	

	SDL_Rect ItemCollision = ThePlayer->GetScreenSpaceCollisionRect();
	SDL_Rect ResultRect;

	SDL_Rect CollisionRect = GetScreenSpaceCollisionRect();

	// Intersect is not inclusive, so widen the collision box
	ItemCollision.x -= 1;
	ItemCollision.w += 2;
	ItemCollision.y -= 1;
	ItemCollision.h += 2;

	if (SDL_IntersectRect(&ItemCollision, &CollisionRect, &ResultRect))
	{
		Interact(ThePlayer);
	}
}

bool Sprite::IsOnGround()
{
	SDL_Rect ScreenSpaceColRect = GetScreenSpaceCustomRect();
	ScreenSpaceColRect.y++;

	return TheMap->CheckCollision(ScreenSpaceColRect, true);
}

SDL_Rect Sprite::GetScreenSpaceRect()
{
	return{ (int)(Rect.x - TheMap->GetScrollX()), (int)(Rect.y - TheMap->GetScrollY()), Rect.w, Rect.h };
}

SDL_Texture* Sprite::GetTexture(int ResourceNum)
{
	if (ResourceNum == 2)
	{
		return Texture2;
	}

	return Texture;
}
