#pragma once

#include "TextBox.h"

struct SaveData
{
	int SpecialEvents[SPECIAL_EVENT_LAST + 1];
	int NormalExits[10];
	int SecretExits[10];

	int GameJustFakeCrashed;
	int BeatGame1;
	int BeatGame2;
};

struct LevelInfo
{
	string FileName;
	string DisplayName;
	int NextLevel;
	int NextSecretLevel;

	vector<eSpecialEvent> SpecialEvents;
};

enum eGameState
{
	STATE_PRE_LEVEL = 0,
	STATE_LEVEL_PLAY,
	STATE_RIDING_FLAG_POLE,
	STATE_TIMER_AWARD,
	STATE_FLAG,
	STATE_FIREWORKS,
	STATE_POST_LEVEL
};

class Game
{
	SaveData TheSaveData;
	struct ControlTrigger ActiveCheckpointControl;
	int CurrentLevel;	
	bool bLevelComplete;
	eGameState GameState;
	int PostLevelCountDown;
	int NumFireworks;
	int CurrentFirework;
	int FireworkCountDown;	
	Mix_Music* CurrentMusic;
	bool bSecretExit;
	int NumberOfTimesPortaled;
	vector<SDL_Point> GatheredRedCoinLocations;
	TextBox* TheTextBox;

public:
	Game();

	void LoadCurrentLevel();

	void StartGame();
	void EndGame();

	void StartLevel();	
	void EndLevel();
	void EnforceWarpControls(struct WarpExit ControlWarp);

	void HandleControl(ControlTrigger* InControl);

	void OnLevelComplete();

	// Returns true if the player has compelted the level
	bool IsLevelComplete();

	void Tick();
	void Render(SDL_Renderer* Renderer);

	eGameState GetGameState();
	void UpdateTimerAward();	
	void OnGrabFlagPole(bool bSecret);
	void OnPlayerFlagDone();	

	void UpdateFireworks();
	void CancelEndLevel();

	string GetLevelName();

	Mix_Music* GetMusic();

	void HandleSpecialEvent(eSpecialEvent Event);
	void WriteSaveFile();
	void ReadSaveFile();

	void DetermineCurrentLevel();
	string GetWorldName();
	int GetCurrentLevelIndex();
	void AddGatheredRedCoinLocation(SDL_Point RedCoinLocation);
	bool HasRedCoinBeenGathered(int TileX, int TileY);
	int GetNumberOfRedCoinsFound() { return GatheredRedCoinLocations.size(); }
	void DoTextBox(int InPosX, int InPosY, int InWidth, int InHeight, string InText);

	bool IsDoingTextBox();
};