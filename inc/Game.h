#pragma once

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
	struct ControlTrigger ActiveCheckpointControl;
	int CurrentLevel;
	bool bLevelComplete;
	eGameState GameState;
	int PostLevelCountDown;
public:
	Game();

	void LoadCurrentLevel();

	void StartGame();
	void EndGame();

	void StartLevel();	
	void EndLevel();
	void EnforceWarpControls(WarpExit ControlWarp);

	void HandleControl(ControlTrigger* InControl);

	void OnLevelComplete();

	// Returns true if the player has compelted the level
	bool IsLevelComplete();

	void Tick();

	eGameState GetGameState();
	void UpdateTimerAward();	
	void OnGrabFlagPole();
	void OnPlayerFlagDone();	
};