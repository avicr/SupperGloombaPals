#include "../inc/Globals.h"
#include "../inc/TMXMap.h"
#include "../inc/Game.h"
#include "../inc/SimpleSprites.h"
#include "../inc/SpriteList.h"

Game::Game()
{
	PostLevelCountDown = IN_CASTLE_FRAMES;
	GameState = STATE_PRE_LEVEL;
	CurrentLevel = 0;
	bLevelComplete = false;
}

void Game::LoadCurrentLevel()
{
	TheMap->ReadMap(Levels[CurrentLevel].FileName.c_str());
}

void Game::EndGame()
{
	ActiveCheckpointControl = ControlTrigger();
}

void Game::StartLevel()
{	
	bSecretExit = false;
	PostLevelCountDown = IN_CASTLE_FRAMES;
	bLevelComplete = false;
	GameState = STATE_LEVEL_PLAY;
	ThePlayer->BeginLevel();	
	TheMap->StartLevel();

	int FirstWarpIndex = TheMap->GetWarpIndex(ActiveCheckpointControl.WarpID);
	WarpExit InitialWarp;

	if (FirstWarpIndex != -1)
	{
		InitialWarp = TheMap->GetWarps()[FirstWarpIndex].Exits[0];
	}

	EnforceWarpControls(InitialWarp);

	ThePlayer->BeginLevel();
	ThePlayer->SetPosition(InitialWarp.PosX, (InitialWarp.PosY - 1) - (ThePlayer->GetHeight() - 64));
}

//void Game::AdvanceLevel()
//{
//	ActiveCheckpointControl = ControlTrigger();
//}

void Game::EnforceWarpControls(WarpExit ControlWarp)
{	
	BGColor = ControlWarp.ExitBGColor;
	TheMap->EnforceWarpControls(ControlWarp);

	if (ControlWarp.MusicChange != 0)
	{
		CurrentMusic = GetMusicFromID(ControlWarp.MusicChange);
		Mix_PlayMusic(CurrentMusic, -1);
	}
}

// Called to clean up the map and stuff
void Game::EndLevel()
{		
	GameState = STATE_TIMER_AWARD;
	SimpleSprites.clear();
	EnemySprites.clear();
	ItemSprites.clear();
	delete TheMap;
	TheMap = new TMXMap();
}

void Game::HandleControl(ControlTrigger* Control)
{
	if (Control->bIsCheckPoint)
	{
		ActiveCheckpointControl = *Control;
	}
	else
	{
		int FirstWarpIndex = TheMap->GetWarpIndex(Control->WarpID);
		WarpExit ControlWarp;

		if (FirstWarpIndex != -1)
		{
			ControlWarp = TheMap->GetWarps()[FirstWarpIndex].Exits[0];
		}
		EnforceWarpControls(ControlWarp);
	}
}

void Game::OnLevelComplete()
{
	GameState = STATE_TIMER_AWARD;
	ActiveCheckpointControl.WarpID = 0;
}

bool Game::IsLevelComplete()
{
	return bLevelComplete;
}

void Game::Tick()
{
	if (GameState == STATE_TIMER_AWARD)
	{
		UpdateTimerAward();
	}
	else if (GameState == STATE_FIREWORKS)
	{
		UpdateFireworks();
	}
	else if(GameState == STATE_POST_LEVEL)
	{
		PostLevelCountDown--;		

		if (!Mix_PlayingMusic() && PostLevelCountDown <= 0)
		{
			if (bSecretExit)
			{
				CurrentLevel = Levels[CurrentLevel].NextSecretLevel;
			}
			else
			{
				CurrentLevel = Levels[CurrentLevel].NextLevel;
			}

			if (CurrentLevel == -1)
			{
				exit(1);
			}
			bLevelComplete = true;
		}
	}
}

string Game::GetLevelName()
{
	return Levels[CurrentLevel].DisplayName;
}

void Game::UpdateTimerAward()
{
	if (TheMap->TradeTimeForPoints(1) <= 0)
	{
		//bLevelComplete = true;
		SDL_Point FlagPosition = TheMap->GetPlayerFlagPosition();
		SimpleSprites.push_back(new PlayerFlagSprite(FlagPosition.x, FlagPosition.y));		
		GameState = STATE_FLAG;		
	}	
}

eGameState Game::GetGameState()
{
	return GameState;
}


void Game::OnGrabFlagPole(bool bSecret)
{
	bSecretExit = bSecret;
	GameState = STATE_RIDING_FLAG_POLE;
	Mix_HaltMusic();
	Mix_PlayChannel(CHANNEL_FLAG_POLE, FlagPoleSound, 0);

	// See how many fireworks we need
	char TempChar[10];
	itoa((int)TheMap->GetSecondsLeft(), TempChar, 10);
	char LastDigit = TempChar[strlen(TempChar) - 1];
	NumFireworks = 0;
	CurrentFirework = 0;
	FireworkCountDown = 0;
	if (LastDigit == '1')
	{
		NumFireworks = 1;
	}
	else if (LastDigit == '3')
	{
		NumFireworks = 3;
	}
	else if (LastDigit == '6')
	{
		NumFireworks = 6;
	}
}

void Game::OnPlayerFlagDone()
{
	/*if (FireWorks)
	{

	}*/
	GameState = STATE_FIREWORKS;	
}

void Game::UpdateFireworks()
{
	if (FireworkCountDown == 0)
	{
		if (CurrentFirework < NumFireworks)
		{
			Sprite* Explosion = new Sprite(GResourceManager->ExplosionTexture->Texture);

			Explosion->SetPosition(FireworkLocations[CurrentFirework].x + TheMap->GetScrollX(), FireworkLocations[CurrentFirework].y + TheMap->GetScrollY());
			Explosion->SetWidth(64);
			Explosion->SetHeight(64);
			Explosion->PlayAnimation(GResourceManager->FireworkAnimation, false);			
			SimpleSprites.push_back(Explosion);

			Mix_PlayChannel(CHANNEL_BUMP, FireworkSound, 0);
			FireworkCountDown = 36;
			CurrentFirework++;
		}
		else
		{
			GameState = STATE_POST_LEVEL;
		}
	}
	else
	{
		FireworkCountDown--;
	}
}

void Game::CancelEndLevel()
{
	PostLevelCountDown = IN_CASTLE_FRAMES;
	bLevelComplete = false;
	GameState = STATE_LEVEL_PLAY;
	ThePlayer->BeginLevel();
}

Mix_Music* Game::GetMusic()
{

	return CurrentMusic;
}