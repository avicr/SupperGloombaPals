#include "../inc/Globals.h"
#include "../inc/TMXMap.h"
#include "../inc/Game.h"
#include "../inc/SimpleSprites.h"
#include "../inc/SpriteList.h"
#include <iostream>
#include <fstream>

Game::Game()
{
	PostLevelCountDown = IN_CASTLE_FRAMES;
	GameState = STATE_PRE_LEVEL;
	CurrentLevel = 0;
	bLevelComplete = false;

	StartGame();
}

void Game::LoadCurrentLevel()
{
	TheMap->ReadMap(Levels[CurrentLevel].FileName.c_str());
}

void Game::StartGame()
{
	ReadSaveFile();
	DetermineCurrentLevel();
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
	WriteSaveFile();
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
				TheSaveData.SecretExits[CurrentLevel] = SecretExitKeys[CurrentLevel];
				CurrentLevel = Levels[CurrentLevel].NextSecretLevel;
			}
			else
			{
				TheSaveData.NormalExits[CurrentLevel] = NormalExitKeys[CurrentLevel];
				CurrentLevel = Levels[CurrentLevel].NextLevel;
			}
			
			bLevelComplete = true;

			if (CurrentLevel == -1)
			{
				exit(1);
			}
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

	if (bSecret)
	{
		SimpleSprites.push_back(new EventSprite("Secret exit found"));
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

void Game::HandleSpecialEvent(eSpecialEvent Event)
{	
	if (Event == SPECIAL_EVENT_CANCEL_CASTLE)
	{
		CancelEndLevel();
	}

	// If this was an event, set the index to the key and save
	if (Event != SPECIAL_EVENT_NONE && TheSaveData.SpecialEvents[Event] == 0)
	{
		TheSaveData.SpecialEvents[Event] = SpecialEventKeys[Event];		
		SimpleSprites.push_back(new EventSprite(SpecialEventDescprtions[Event]));
		WriteSaveFile();
	}
}

void Game::WriteSaveFile()
{
	ofstream SaveFile("rend.ext");

	for (int i = 0; i < SPECIAL_EVENT_LAST; i++)
	{
		SaveFile << TheSaveData.SpecialEvents[i]<<endl;		
	}

	for (int i = 0; i < 10; i++)
	{
		SaveFile << TheSaveData.NormalExits[i] << endl;
	}

	for (int i = 0; i < 10; i++)
	{
		SaveFile << TheSaveData.SecretExits[i] << endl;
	}
}

void Game::ReadSaveFile()
{
	ifstream SaveFile("rend.ext");

	for (int i = 0; i < SPECIAL_EVENT_LAST; i++)
	{
		SaveFile >> TheSaveData.SpecialEvents[i];

		// If we don't have the correct key for the event, set it to 0 to signify it hasn't happend
		if (TheSaveData.SpecialEvents[i] != SpecialEventKeys[i])
		{
			TheSaveData.SpecialEvents[i] = 0;
		}
	}

	for (int i = 0; i < 10; i++)
	{
		SaveFile >> TheSaveData.NormalExits[i];

		// If we don't have the correct key for the event, set it to 0 to signify it hasn't happend
		if (TheSaveData.NormalExits[i] != NormalExitKeys[i])
		{
			TheSaveData.NormalExits[i] = 0;
		}
	}

	for (int i = 0; i < 10; i++)
	{
		SaveFile >> TheSaveData.SecretExits[i];

		// If we don't have the correct key for the event, set it to 0 to signify it hasn't happend
		if (TheSaveData.SecretExits[i] != SecretExitKeys[i])
		{
			TheSaveData.SecretExits[i] = 0;
		}
	}
}

void Game::DetermineCurrentLevel()
{
	int NormalIndex = 0;
	int SecretIndex = 0;

	int GreatestCompletedNormalExit = -1;
	int GreatestCompletedSecretExit = -1;


	for (NormalIndex; NormalIndex < 10; NormalIndex++)
	{
		if (TheSaveData.NormalExits[NormalIndex])
		{
			GreatestCompletedNormalExit = NormalIndex > GreatestCompletedNormalExit ? NormalIndex : NormalIndex;

		}
	}

	for (SecretIndex; SecretIndex < 10; SecretIndex++)
	{
		if (TheSaveData.SecretExits[SecretIndex])
		{
			GreatestCompletedSecretExit = SecretIndex > GreatestCompletedSecretExit ? SecretIndex : NormalIndex;

		}
	}


 //	if (TheSaveData.BeatGame1)
	//{
	//	// TODO: Level select!
	//}
	//else 
	if (GreatestCompletedNormalExit > GreatestCompletedSecretExit)
	{
		// If even, this is a normal level exit
		if (GreatestCompletedNormalExit % 2 == 0)
		{
			CurrentLevel = GreatestCompletedNormalExit + 2;
		}
		else
		{
			CurrentLevel = GreatestCompletedNormalExit + 1;
		}
	}
	else if (GreatestCompletedNormalExit <= GreatestCompletedSecretExit)
	{
		// If even, this is a normal level exit
		if (SecretIndex % 2 == 0)
		{
			CurrentLevel = GreatestCompletedSecretExit + 1;
		}
		else
		{
			CurrentLevel = GreatestCompletedSecretExit + 2;
		}
	}
	else if (GreatestCompletedSecretExit == -1 && GreatestCompletedNormalExit == -1)
	{
		CurrentLevel = 0;
	}

	CurrentLevel = 2;
}

string Game::GetWorldName()
{
	return Levels[CurrentLevel].DisplayName;
}

int Game::GetCurrentLevelIndex()
{
	return CurrentLevel;
}