#include "../inc/Globals.h"
#include "../inc/TMXMap.h"
#include "../inc/Game.h"
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
	bLevelComplete = false;
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
	bLevelComplete = true;
}

bool Game::IsLevelComplete()
{
	return bLevelComplete;
}