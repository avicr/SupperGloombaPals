#include "../inc/Globals.h"
#include "../inc/TMXMap.h"
#include "../inc/Game.h"
#include "../inc/SimpleSprites.h"
#include "../inc/SpriteList.h"

char* LevelNames[8] = 
{
	"Resource/TileMaps/Level1_1_64x64.tmx",
	"Resource/TileMaps/Level1_1_64x64.tmx",
	"Resource/TileMaps/Level1_1_64x64.tmx",
	"Resource/TileMaps/Level1_1_64x64.tmx",
	"Resource/TileMaps/Level1_1_64x64.tmx",
	"Resource/TileMaps/Level1_1_64x64.tmx",
	"Resource/TileMaps/Level1_1_64x64.tmx",
	"Resource/TileMaps/Level1_1_64x64.tmx"
};

Game::Game()
{
	PostLevelCountDown = IN_CASTLE_FRAMES;
	GameState = STATE_PRE_LEVEL;
	CurrentLevel = 0;
	bLevelComplete = false;
}

void Game::LoadCurrentLevel()
{
	TheMap->ReadMap(LevelNames[CurrentLevel]);
}

void Game::EndGame()
{
	ActiveCheckpointControl = ControlTrigger();
}

void Game::StartLevel()
{	
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
	Mix_PlayMusic(GetMusicFromID(ControlWarp.MusicChange), -1);
}

// Called to clearn up the map and stuff
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
	else if(GameState == STATE_POST_LEVEL)
	{
		PostLevelCountDown--;

		if (PostLevelCountDown <= 0)
		{
			bLevelComplete = true;
		}
	}
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


void Game::OnGrabFlagPole()
{
	GameState = STATE_RIDING_FLAG_POLE;
}

void Game::OnPlayerFlagDone()
{
	/*if (FireWorks)
	{

	}*/
	GameState = STATE_POST_LEVEL;	
}

