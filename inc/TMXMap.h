#pragma once
#include <SDL.h>
#include "Globals.h"
#include "Animation.h"
#include "../tinyxml/tinyxml.h"
#include <sstream>
#include <vector>
using namespace std;

#define MAX_TILE_OFFSET 4
#define TILE_VELOCITY 0.72

struct TileOffset
{
	float OffsetX = 0;
	float OffsetY = 0;
	float VelocityX = TILE_VELOCITY;
	float VelocityY = TILE_VELOCITY;
	
	TileOffset()
	{
		OffsetX = (rand() % (MAX_TILE_OFFSET * 2)) - 8;
		OffsetY = (rand() % (MAX_TILE_OFFSET * 2)) - 8;

		if (OffsetX > 0)
		{
			VelocityX *= -1;
		}

		if (OffsetY > 0)
		{
			VelocityY *= -1;
		}
	}
};

enum eWarpType
{
	WARP_INSTANT = 0,
	WARP_PIPE_UP,
	WARP_PIPE_DOWN,
	WARP_PIPE_LEFT,
	WARP_PIPE_RIGHT,
	WARP_PIPE_SHOOT_UP,
	WARP_PIPE_RIGHT_SECOND_WINDOW,
	WARP_PIPE_LEFT_FIRST_WINDOW,
	WARP_CONTROL = 255
};

enum eBrickBreakTilesetID
{
	BRICK_TILESET_OVERWORLD = 0,
	BRICK_TILESET_UNDERGROUND,
	BRICK_TILESET_CASTLE
};

struct WarpExit
{
	eDirection ExitDirection = DIRECTION_UP;
	eWarpType WarpType = WARP_INSTANT; 
	SDL_Color ExitBGColor = { 54, 129, 241, 255 };
	bool bLockScrollX = false;
	bool bLockScrollY = true;	
	int MaxScrollY = 0;
	int MaxScrollX = 0;
	bool bNoScrollChange = false;
	bool bChangePositionX = true;
	bool bChangePositionY = true;
	int MusicChange = 0;
	int ScrollOffsetX = 0;
	int ScrollOffsetY = 0;
	int PosX = 0;
	int PosY = 0;
	int Chance = 100;
	int KillY = -1;
	eBrickBreakTilesetID BrickTilesetID = BRICK_TILESET_OVERWORLD;
};

struct WarpEntrance
{
	// If true, no effect other than going down the pipe when true
	bool bQuickTransition = false;

	int PosX = 0;
	int PosY = 0;	
	
	eWarpType WarpType = WARP_INSTANT;
};

struct WarpInfo
{
	int ID;

	vector <WarpEntrance> Entrances;
	vector <WarpExit> Exits;

	WarpInfo(int InID)
	{
		ID = InID;
	}
};

struct ControlTrigger
{
	int PosX = 0;
	int PosY = 0;
	int Width = 64;
	int Height = 64;

	bool bIsCheckPoint = false;
	int WarpID = 0;
	int ID = -1;
	bool bTriggered = false;
	
	bool bResetWhenPlayerLeaves = true;
	enum eSpecialEvent SpecialEvent = SPECIAL_EVENT_NONE;
};

struct MultiCoinManager
{
	int Timer;
	int CoinsRemaining;
	int TileX;
	int TileY;

	MultiCoinManager(int InTileX, int InTileY, int NumCoins) : Timer(0), TileX(InTileX), TileY(InTileY), CoinsRemaining(NumCoins) {}
	bool Tick();
};


enum eTileMetaType
{
	TILE_COLLISION,
	TILE_COIN_BLOCK,
	TILE_POWER_UP,
	TILE_ONE_UP,
	TILE_PIPE_ENTRANCE,
	TILE_PIPE_EXIT,
	TILE_PLAYER_FLAG,
	TILE_COIN,
	TILE_BREAKABLE,	
	TILE_UGLY,
	TILE_MULTI_COIN_BLOCK,
	TILE_SPAWN_GOOMBA,
	TILE_SPAWN_TURTLE,
	TILE_STAR,
	TILE_TRIGGER,
	TILE_MAGIC_MUSHROOM,
	TILE_DESTROY_WITH_FIRE,
	TILE_DESTROY_WITH_FIRE_OR_BUMP,
	TILE_FLAG_POLE,
	TILE_BREAK_ON_TOUCH,	
	TILE_RED_COIN_BLOCK,
	TILE_RED_COIN,
	TILE_NONE = 255
};

struct TileInfo
{
	// In tile coordinates!
	SDL_Point Location;

	eTileMetaType MetaTileType = TILE_NONE;
	int ForegroundTile = 0;

	TileInfo(SDL_Point Loc, eTileMetaType MetaTile, int Foreground) : Location(Loc), MetaTileType(MetaTile), ForegroundTile(Foreground) {};
};

struct TMXLayer
{
	char Name[255];
	int Width;
	int Height;	

	int** TileData = NULL;

	TMXLayer(char *InName, int InWidth, int InHeight, const char* LayerData)
	{
		stringstream ss(LayerData);

		strcpy_s(Name, InName);		
		Width = InWidth;
		Height = InHeight;

		TileData = new int*[Height];
		for (int y = 0; y < Height; y++)
		{
			TileData[y] = new int[Width];
			
			for (int x = 0; x < Width; x++)
			{

				string substr;
				getline(ss, substr, ',');									
				
				// Subtract one so we can use 0 based index
				TileData[y][x] = std::stoi(substr) - 1;			
			}			
		}		
	}
	~TMXLayer()
	{
		if (TileData)
		{
			for (int i = 0; i < Height; i++)
			{
				delete[] TileData[i];
			}

			delete[] TileData;
		}
	}
};

struct TMXFrame
{
	int TileID;
	int FrameDuration;

	TMXFrame(int ID, int Duration)
	{
		TileID = ID;
		FrameDuration = Duration;
	}
};

class TMXAnimation
{
	vector<TMXFrame> Frames;
	int TileID;
	int FrameCountDown;
	int CurrentFrameIndex;

public:
	TMXAnimation(int TileID);
	void Tick();
	int GetCurrentTileID();
	void AddFrame(int FrameTileID, int FrameDuration);
	int GetOriginalTileID();
};

struct TMXTileset
{
	char Name[255];	
	int TilePixelWidth;
	int TilePixelHeight;
	int FirstGID;
	int NumColumns;
	vector<TMXAnimation> TileAnimations;

	SDL_Texture* TilesetTexture;

	int GetTileIDFromGID(int GID)
	{
		int TileID = GID - FirstGID;

		// Check the animations 
		for (int i = 0; i < TileAnimations.size(); i++)
		{
			if (TileID == 65)
			{
				TileID = TileID;
			}
			if (TileAnimations[i].GetOriginalTileID() == TileID)
			{
				return TileAnimations[i].GetCurrentTileID();
			}
		}

		return TileID;
	}

	void TickAnimations()
	{
		for (int i = 0; i < TileAnimations.size(); i++)
		{
			TileAnimations[i].Tick();
		}
	}

	TMXTileset(int InFirstGID, const char* InSource)
	{
		string TxPath = "Resource/Tilemaps/";
		TxPath += InSource;
		TiXmlDocument Doc(TxPath.c_str());
		Doc.LoadFile();
		TiXmlElement* pElem = Doc.FirstChildElement();

		if (!pElem)
		{
			printf("ERROR READING %s", InSource);
			return;
		}

		// Subtract one so we can use 0 based index
		FirstGID = InFirstGID - 1;

		strcpy_s(Name, pElem->Attribute("name"));
		pElem->QueryIntAttribute("tilewidth", &TilePixelWidth);
		pElem->QueryIntAttribute("tileheight", &TilePixelHeight);
		pElem->QueryIntAttribute("columns", &NumColumns);

		// Get the <image>
		pElem = pElem->FirstChildElement();

		string ImagePath = pElem->Attribute("source");
		ImagePath = ImagePath.replace(0, 2, "Resource");
				
		SDL_Surface * Image = SDL_LoadBMP(ImagePath.c_str());
		SDL_Log("Loaded: %d", Image);
		SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0, 0xFF));
		TilesetTexture = SDL_CreateTextureFromSurface(GetRenderer(), Image);		

		SDL_FreeSurface(Image);
	}

	~TMXTileset()
	{
		if (TilesetTexture)
		{
			SDL_DestroyTexture(TilesetTexture);
			TilesetTexture = NULL;
		}

	}

};

class TMXMap
{
protected:

	TileOffset TileOffsets[16][15];
	vector<MultiCoinManager> CoinManagers;
	vector<WarpInfo> Warps;
	vector <ControlTrigger*> ControlTiles;

	bool bPlayingLevel;
	bool bLockScrollX;
	bool bLockScrollY;

	double ScrollX;
	double ScrollY;

	int PlayerFlagX;
	int PlayerFlagY;

	// If the player is below this, kill them.  In Pixels	
	int KillY;

	int MaxScrollX;
	int MaxScrollY;

	int TilePixelWidth;
	int TilePixelHeight;

	// Width in Tiles
	int Width;

	// Height In Tiles
	int Height;

	int MetaTileGID;
	int StandardTileGID;

	string WorldName;
	double SecondsLeft;	

	// Which type of brick stuff to spawn
	eBrickBreakTilesetID BrickTilesetID;

	vector<TMXTileset*> Tilesets;
	vector<TMXLayer*> Layers;
	TMXTileset* MetaTileset;
	TMXLayer* CollisionLayer;
	TMXLayer* ForegroundLayer;

	TMXTileset* LoadTileset(TiXmlElement* TilesetElement);
	TMXLayer* LoadLayer(TiXmlElement* TilesetElement);	
	void LoadObjects(TiXmlElement* ObjectElement);
	void LoadExit(TiXmlElement* TilesetElement);
	void LoadEntrance(TiXmlElement* TilesetElement);
	void LoadControl(TiXmlElement* ControlElement);
	void ReleaseAssets();
	
public:
	void ReadMap(const char* FileName);
	void Render(SDL_Renderer* Renderer, int ScreenX, int ScreenY, int SourceWidth, int SourceHeight);
	void RenderLayer(const TMXLayer* Layer, SDL_Renderer* Renderer, int ScreenX, int ScreenY, int SourceWidth, int SourceHeight);
	TMXTileset* GetTilesetFromGID(int GID);

	void AdjustScrollX(double Amount);
	void AdjustScrollY(double Amount);
	void DoWarp(WarpExit WarpExit);
	void EnforceWarpControls(WarpExit Warp);

	double GetScrollX() { return ScrollX; }
	double GetScrollY() { return ScrollY; }
	void ToggleRenderCollision();

	eTileMetaType GetMetaTileType(int TileX, int TileY);

	/** TileLoc - X, Y tile that the player collided with*/
	bool CheckCollision(SDL_Rect Rect, vector<TileInfo>& TileLoc, bool bIgnoreBorders = false, bool bSolidTilesOnly = false);	
	bool CheckCollision(SDL_Rect Rect, bool bIgnoreBorders = false);
	bool IsHiddenBlockTile(int ID);
	bool IsBreakableBlockTile(int ID);
	bool IsCoinTile(int ID);
	bool IsCollidableTile(int MetaTileID, int TileX, int TileY, SDL_Point TestPoint = { -1, -1 });

	void SetMetaLayerTile(int TileX, int TileY, eTileMetaType TileType);
	void SetForegroundTile(int TileX, int TileY, int TileID);
	void HandleCollision(int TileX, int TileY, bool bCanBreakBricks = false);

	void Tick(double DeltaTime);

	int GetCoinManagerIndex(int TileX, int TileY);
	int GetWarpIndex(int ID);
	bool InMapWindow(int X, int Y);
	bool InMapWindow(int X, int Y, int Width, int Height);
	vector<WarpInfo>GetWarps();
	vector<ControlTrigger*>GetControlTiles() { return ControlTiles; }
	int GetHeightInPixels() { return Height * TilePixelHeight; }
	int GetSecondsLeft() { return (int)round(SecondsLeft); }
	string GetWorldName() { return WorldName; }
	bool LevelPlaying() { return bPlayingLevel; }
	void StartLevel();
	void EndLevel();
	int GetKillY() { return KillY; }
	int GetWidth() { return Width * TilePixelWidth; }
	int GetHeight() { return Height * TilePixelHeight; }
	bool IsDestroyableByFireTile(int ID);
	double TradeTimeForPoints(int Amount);
	SDL_Point GetPlayerFlagPosition() { return{ PlayerFlagX, PlayerFlagY }; }
	eBrickBreakTilesetID GetBrickTilesetID() { return BrickTilesetID; }
	AnimationResource* GetBrickBounceAnimForBrickTileset();
	AnimationResource* GetEmptyBlockBounceAnimForBrickTileset();

	SDL_Rect GetVisibleWindow();	

	TMXMap();
	~TMXMap();
};
