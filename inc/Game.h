#pragma once

class Game
{
	struct ControlTrigger ActiveCheckpointControl;
	int CurrentLevel;
	bool bLevelComplete;

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
};