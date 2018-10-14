#include "../inc/TMXMap.h"
#include "../tinyxml/tinyxml.h"
#include "../inc/SpriteList.h"
#include "../inc/EnemySpawnPoint.h"
#include "../inc/EmptyBlockSprite.h"
#include "../inc/Game.h"
#include "../inc/SimpleSprites.h"
#include "../inc/PlayerSprite.h"
#include "../inc/ItemSprite.h"
#include <stdlib.h>
#include <sstream>
#include <algorithm>

using namespace std;

TMXLayer::TMXLayer(char *InName, int InWidth, int InHeight, const char* LayerData)
{
	stringstream ss(LayerData);
	bool bIsCollisionLayer = false;

	if (strcmp(InName, "Collision") == 0)
	{
		bIsCollisionLayer = true;
	}

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
						
			if (bIsCollisionLayer)
			{
				if (TileData[y][x] - TheMap->GetMetaTileGID() == TILE_RED_COIN)
				{
					if (TheGame->HasRedCoinBeenGathered(x, y))
					{
						TileData[y][x] = TILE_COIN + TheMap->GetMetaTileGID();
					}
				}
				else if (TileData[y][x] - TheMap->GetMetaTileGID() == TILE_RED_COIN_BLOCK)
				{
					if (TheGame->HasRedCoinBeenGathered(x, y))
					{
						TileData[y][x] = TILE_COIN_BLOCK + TheMap->GetMetaTileGID();
					}
				}
			}
		}
	}
}

bool MultiCoinManager::Tick()
{	
	Timer++;

	if (Timer % 60 == 0)
	{
		CoinsRemaining--;				
	}
	return false;
	
}

TMXAnimation::TMXAnimation(int InTileID)
{
	TileID = InTileID;
	CurrentFrameIndex = 0;
}

void TMXAnimation::Tick()
{
	FrameCountDown--;

	if (FrameCountDown <= 0)
	{
		CurrentFrameIndex++;

		if (CurrentFrameIndex >= Frames.size())
		{
			CurrentFrameIndex = 0;
		}

		FrameCountDown = Frames[CurrentFrameIndex].FrameDuration;
	}
}

int TMXAnimation::GetCurrentTileID()
{
	return Frames[CurrentFrameIndex].TileID;
}

void TMXAnimation::AddFrame(int FrameTileID, int FrameDuration)
{
	Frames.push_back(TMXFrame(FrameTileID, FrameDuration));

	// If this is the first frame, setup the count down
	if (Frames.size() == 1)
	{
		FrameCountDown = Frames[0].FrameDuration;
	}
}

int TMXAnimation::GetOriginalTileID()
{
	return TileID;
}

void TMXMap::ReadMap(const char* FileName)
{	
	TiXmlDocument Doc(FileName);
	char SourcePath[255];
	
	if (!Doc.LoadFile())
	{
		return;
	}

	TiXmlHandle hDoc(&Doc);
	TiXmlElement* pElem;
	TiXmlElement* MapElement;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();
	
	MapElement = pElem;
	
	MapElement->QueryIntAttribute("width", &Width);
	MapElement->QueryIntAttribute("height", &Height);
	MapElement->QueryIntAttribute("tilewidth", &TilePixelWidth);
	MapElement->QueryIntAttribute("tileheight", &TilePixelHeight);

	pElem = MapElement->FirstChildElement();
	
	while (pElem)
	{		
		// If this is a tileset, load the tileset
		if (strcmp(pElem->Value(), "tileset") == 0)
		{			
			strcpy_s(SourcePath, pElem->Attribute("source"));

			if (strcmp(SourcePath, "MetaTiles.tsx") == 0)
			{
				pElem->QueryIntAttribute("firstgid", &MetaTileGID);
				// Adjust for off by one garbage
				MetaTileGID--;
				MetaTileset = LoadTileset(pElem);
				Tilesets.push_back(MetaTileset);

				SDL_SetTextureBlendMode(MetaTileset->TilesetTexture, SDL_BLENDMODE_BLEND);
				SDL_SetTextureAlphaMod(MetaTileset->TilesetTexture, 128);
			}
			else
			{
				if (strcmp(SourcePath, "Basic64x64.tsx") == 0)
				{
					pElem->QueryIntAttribute("firstgid", &StandardTileGID);
					
					// Adjust for off by one garbage
					StandardTileGID--;
				}
				Tilesets.push_back(LoadTileset(pElem));
			}
		}
		else if (strcmp(pElem->Value(), "layer") == 0)
		{				
			if (strcmp(pElem->Attribute("name"), "Collision") == 0)
			{
				CollisionLayer = LoadLayer(pElem);
			}			
			else
			{
				Layers.push_back(LoadLayer(pElem));
				
				if (strcmp(pElem->Attribute("name"), "Background") == 0)
				{
					BackgroundLayer = Layers[Layers.size() - 1];
				}
				else if (strcmp(pElem->Attribute("name"), "Foreground") == 0)
				{
					ForegroundLayer = Layers[Layers.size() - 1];

					// TEST
					TMXTileset* StandardTiles = GetTilesetFromGID(StandardTileGID);
					TMXAnimation NewAnimation(32);
					NewAnimation.AddFrame(32, 24);
					NewAnimation.AddFrame(27, 8);
					NewAnimation.AddFrame(28, 8);
					NewAnimation.AddFrame(27, 8);
					
					StandardTiles->TileAnimations.push_back(NewAnimation);

					NewAnimation = TMXAnimation(43);
					NewAnimation.AddFrame(43, 24);
					NewAnimation.AddFrame(44, 8);
					NewAnimation.AddFrame(45, 8);
					NewAnimation.AddFrame(44, 8);

					StandardTiles->TileAnimations.push_back(NewAnimation);

					// G.
					NewAnimation = TMXAnimation(66);
					NewAnimation.AddFrame(66, 8);
					NewAnimation.AddFrame(63, 40);
					//NewAnimation.AddFrame(63, 40);
					StandardTiles->TileAnimations.push_back(NewAnimation);

					// ..
					NewAnimation = TMXAnimation(65);
					NewAnimation.AddFrame(56, 16);
					NewAnimation.AddFrame(65, 16);
					NewAnimation.AddFrame(64, 16);

					StandardTiles->TileAnimations.push_back(NewAnimation);
				}
			}			
		}
		else if (strcmp(pElem->Value(), "objectgroup") == 0)
		{
			if (strcmp(pElem->Attribute("name"), "Objects") == 0)
			{
				LoadObjects(pElem);
			}
		}

		pElem = pElem->NextSiblingElement();
	}

	std::reverse(Tilesets.begin(), Tilesets.end());
}

TMXTileset* TMXMap::LoadTileset(TiXmlElement* TilesetElement)
{
	int FirstGID;

	TilesetElement->QueryIntAttribute("firstgid", &FirstGID);	
	return new TMXTileset(FirstGID, TilesetElement->Attribute("source"));
}

TMXLayer* TMXMap::LoadLayer(TiXmlElement* LayerElement)
{
	char Name[255];
	int Width;
	int Height;

	strcpy_s(Name, LayerElement->Attribute("name"));
	LayerElement->QueryIntAttribute("width", &Width);
	LayerElement->QueryIntAttribute("height", &Height);
	const char* LayerData = LayerElement->FirstChildElement()->GetText();
		
	return new TMXLayer(Name, Width, Height, LayerData);
}

void TMXMap::LoadObjects(TiXmlElement* ObjectElement)
{
	int GID = -1;
	TiXmlElement* ObjElem = ObjectElement->FirstChildElement("object");

	while (ObjElem)
	{
		int ID;
		ObjElem->QueryIntAttribute("id", &ID);
		ObjElem->QueryIntAttribute("gid", &GID);
		GID--;

		SDL_Log(ObjElem->Attribute("gid"));
		// HACK
		int TileId = GID - MetaTileGID;
	
		// Player flag
		if (TileId == TILE_PLAYER_FLAG)
		{
			ObjElem->QueryIntAttribute("x", &PlayerFlagX);
			ObjElem->QueryIntAttribute("y", &PlayerFlagY);

			PlayerFlagY -= 64;
		}

		// Flag pole
		if (TileId == TILE_FLAG_POLE)
		{
			int X;
			int Y;
			bool bIsSecretExit = false;

			ObjElem->QueryIntAttribute("x", &X);
			ObjElem->QueryIntAttribute("y", &Y);
		
			TiXmlElement* PropElem = ObjElem->FirstChildElement("properties");

			if (PropElem)
			{
				PropElem = PropElem->FirstChildElement("property");
			}

			while (PropElem)
			{
				if (strcmp(PropElem->Attribute("name"), "secretexit") == 0)
				{
					PropElem->QueryBoolAttribute("value", &bIsSecretExit);
				}
				PropElem = PropElem->NextSiblingElement();
			}

			SimpleSprites.push_back(new FlagPoleSprite(X, Y, bIsSecretExit));
		}

		// Spawn points
		if (TileId == TILE_SPAWN_GOOMBA || TileId == TILE_SPAWN_TURTLE)
		{
			int X;
			int Y;

			ObjElem->QueryIntAttribute("x", &X);
			ObjElem->QueryIntAttribute("y", &Y);
			
			int EnemyType = 0;

			// Find the ENEMY_ID property if available...		
			TiXmlElement* PropElem = ObjElem->FirstChildElement("properties");

			if (PropElem)
			{
				PropElem = PropElem->FirstChildElement("property");
			}

			while (PropElem)
			{
				if (strcmp(PropElem->Attribute("name"), "ENEMY_ID") == 0)
				{
					PropElem->QueryIntAttribute("value", &EnemyType);					
				}				
				PropElem = PropElem->NextSiblingElement();
			}	

			EnemySpawnPoint* NewEnemySpawnPoint;
			if (EnemyType == ENEMY_BULLET)
			{
				NewEnemySpawnPoint = new TimedEnemySpawnPoint((eEnemyType)EnemyType, X, Y - 64);
			}
			else
			{
				NewEnemySpawnPoint = new EnemySpawnPoint((eEnemyType)EnemyType, X, Y - 64);
			}
 			SimpleSprites.push_back(NewEnemySpawnPoint);
		}
		// Warp entrances
		else if (TileId == 4)
		{
			LoadEntrance(ObjElem);
		}
		// Warp exits
		else if (TileId == 5)
		{						
			LoadExit(ObjElem);
		}
		else if (TileId == 14)
		{
			LoadControl(ObjElem);
		}

		ObjElem = ObjElem->NextSiblingElement("object");
	}
}

void TMXMap::LoadEntrance(TiXmlElement* ObjectElement)
{
	WarpEntrance NewEntrance;
	int ID = 0;
	
	ObjectElement->QueryIntAttribute("x", &NewEntrance.PosX);
	ObjectElement->QueryIntAttribute("y", &NewEntrance.PosY);

	NewEntrance.PosY -= 64;

	int EnemyType = 0;

	// Find the ENEMY_ID property if available...		
	TiXmlElement* PropElem = ObjectElement->FirstChildElement("properties");

	if (PropElem)
	{
		PropElem = PropElem->FirstChildElement("property");
	}

	while (PropElem)
	{
		if (strcmp(PropElem->Attribute("name"), "warpid") == 0)
		{
			PropElem->QueryIntAttribute("value", &ID);
		}
		else if (strcmp(PropElem->Attribute("name"), "warptype") == 0)
		{
			int WarpType = 0;
			PropElem->QueryIntAttribute("value", &WarpType);

			NewEntrance.WarpType = (eWarpType)WarpType;
		}
		else if (strcmp(PropElem->Attribute("name"), "quicktransition") == 0)
		{
			int WarpTransition = 0;
			PropElem->QueryIntAttribute("value", &WarpTransition);

			NewEntrance.bQuickTransition = WarpTransition;
		}
		PropElem = PropElem->NextSiblingElement();
	}

	int WarpIndex = GetWarpIndex(ID);

	if (WarpIndex == -1)
	{
		WarpIndex = Warps.size();
		Warps.push_back(WarpInfo(ID));
	}

	Warps[WarpIndex].Entrances.push_back(NewEntrance);
}

void TMXMap::LoadControl(TiXmlElement* ControlElement)
{
	ControlTrigger* NewControl = new ControlTrigger();
	int ID = 0;

	ControlElement->QueryIntAttribute("x", &NewControl->PosX);
	ControlElement->QueryIntAttribute("y", &NewControl->PosY);
	ControlElement->QueryIntAttribute("width", &NewControl->Width);
	ControlElement->QueryIntAttribute("height", &NewControl->Height);
	ControlElement->QueryIntAttribute("id", &NewControl->ID);

	NewControl->PosY -= NewControl->Height;


	// Find the ENEMY_ID property if available...		
	TiXmlElement* PropElem = ControlElement->FirstChildElement("properties");

	if (PropElem)
	{
		PropElem = PropElem->FirstChildElement("property");
	}

	while (PropElem)
	{
		if (strcmp(PropElem->Attribute("name"), "warpid") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewControl->WarpID);
		}
		else if (strcmp(PropElem->Attribute("name"), "ischeckpoint") == 0)
		{
			PropElem->QueryBoolAttribute("value", &NewControl->bIsCheckPoint);
		}
		else if (strcmp(PropElem->Attribute("name"), "resetwhenplayerleaves") == 0)
		{
			PropElem->QueryBoolAttribute("value", &NewControl->bResetWhenPlayerLeaves);
		}
		else if (strcmp(PropElem->Attribute("name"), "specialevent") == 0)
		{
			int SpecialEvent = 0;
			PropElem->QueryIntAttribute("value", &SpecialEvent);

			NewControl->SpecialEvent = (eSpecialEvent) SpecialEvent;
		}
		else if (strcmp(PropElem->Attribute("name"), "event") == 0)
		{
			int Event = 0;
			PropElem->QueryIntAttribute("value", &Event);

			NewControl->Event = (eControlEvent)Event;
		}
		
		PropElem = PropElem->NextSiblingElement();
	}	

	ControlTiles.push_back(NewControl);
}

void TMXMap::LoadExit(TiXmlElement* ObjectElement)
{
	WarpExit NewExit;
	int ID = 0;

	ObjectElement->QueryIntAttribute("x", &NewExit.PosX);
	ObjectElement->QueryIntAttribute("y", &NewExit.PosY);
	NewExit.PosY -= 64;
	int EnemyType = 0;

	// Find the ENEMY_ID property if available...		
	TiXmlElement* PropElem = ObjectElement->FirstChildElement("properties");

	if (PropElem)
	{
		PropElem = PropElem->FirstChildElement("property");
	}

	while (PropElem)
	{
		if (strcmp(PropElem->Attribute("name"), "warpid") == 0)
		{
			PropElem->QueryIntAttribute("value", &ID);
		}
		else if (strcmp(PropElem->Attribute("name"), "exittype") == 0)
		{
			int WarpType = 0;
			PropElem->QueryIntAttribute("value", &WarpType);

			NewExit.WarpType = (eWarpType)WarpType;
		}
		else if (strcmp(PropElem->Attribute("name"), "chance") == 0)
		{			
			PropElem->QueryIntAttribute("value", &NewExit.Chance);			
		}
		else if (strcmp(PropElem->Attribute("name"), "exitdirection") == 0)
		{
			int ExitDirection;
			PropElem->QueryIntAttribute("value", &ExitDirection);

			NewExit.ExitDirection = (eDirection)ExitDirection;
		}
		else if (strcmp(PropElem->Attribute("name"), "lockx") == 0)
		{
			int LockX;
			PropElem->QueryIntAttribute("value", &LockX);

			if (LockX)
			{
				NewExit.ScrollLockFlags |= MAP_LOCK_POSITIVE_X;
			}
			else
			{
				NewExit.ScrollLockFlags &= ~MAP_LOCK_POSITIVE_X;
			}
		}
		else if (strcmp(PropElem->Attribute("name"), "locky") == 0)
		{
			int LockY;
			PropElem->QueryIntAttribute("value", &LockY);

			if (LockY)
			{
				NewExit.ScrollLockFlags |= MAP_LOCK_POSITIVE_Y;
			}
			else
			{
				NewExit.ScrollLockFlags &= ~MAP_LOCK_POSITIVE_Y;
			}
		}
		else if (strcmp(PropElem->Attribute("name"), "locknegativex") == 0)
		{			
			bool bLockNegativeX;
			PropElem->QueryBoolAttribute("value", &bLockNegativeX);

			if (bLockNegativeX)
			{
				NewExit.ScrollLockFlags |= MAP_LOCK_NEGATIVE_X;
			}
			else
			{
				NewExit.ScrollLockFlags &= ~MAP_LOCK_NEGATIVE_X;
			}
		}
		else if (strcmp(PropElem->Attribute("name"), "locknegativey") == 0)
		{
			bool bLockNegativeY;
			PropElem->QueryBoolAttribute("value", &bLockNegativeY);

			if (bLockNegativeY)
			{
				NewExit.ScrollLockFlags |= MAP_LOCK_NEGATIVE_Y;
			}
			else
			{
				NewExit.ScrollLockFlags &= ~MAP_LOCK_NEGATIVE_Y;
			}
		}
		else if (strcmp(PropElem->Attribute("name"), "scrollvelocityx") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewExit.ScrollVelocityX);
		}
		else if (strcmp(PropElem->Attribute("name"), "scrollvelocityy") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewExit.ScrollVelocityY);
		}
		else if (strcmp(PropElem->Attribute("name"), "maxscrolly") == 0)
		{			
			PropElem->QueryIntAttribute("value", &NewExit.MaxScrollY);			
		}
		else if (strcmp(PropElem->Attribute("name"), "maxscrollx") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewExit.MaxScrollX);
		}
		else if (strcmp(PropElem->Attribute("name"), "musicchange") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewExit.MusicChange);
		}
		else if (strcmp(PropElem->Attribute("name"), "scrolloffsetx") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewExit.ScrollOffsetX);
		}
		else if (strcmp(PropElem->Attribute("name"), "scrolloffsety") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewExit.ScrollOffsetY);
		}
		else if (strcmp(PropElem->Attribute("name"), "noscrollchange") == 0)
		{
			int DoNotScroll;
			PropElem->QueryIntAttribute("value", &DoNotScroll);
			NewExit.bNoScrollChange = DoNotScroll;
		}
		else if (strcmp(PropElem->Attribute("name"), "bricktilesetid") == 0)
		{
			int BirckTileSet;
			PropElem->QueryIntAttribute("value", &BirckTileSet);
			NewExit.BrickTilesetID = (eBrickBreakTilesetID) BirckTileSet;
		}
		else if (strcmp(PropElem->Attribute("name"), "positionchangex") == 0)
		{			
			PropElem->QueryBoolAttribute("value", &NewExit.bChangePositionX);			
		}
		else if (strcmp(PropElem->Attribute("name"), "positionchangey") == 0)
		{
			PropElem->QueryBoolAttribute("value", &NewExit.bChangePositionY);
		}
		else if (strcmp(PropElem->Attribute("name"), "killy") == 0)
		{
			PropElem->QueryIntAttribute("value", &NewExit.KillY);
		}
		else if (strcmp(PropElem->Attribute("name"), "bgcolor") == 0)
		{
			char *Color = new char[10];
			char TempColor[3];
			TempColor[2] = 0;

			strcpy (Color, PropElem->Attribute("value"));			
			
			Color++;
			TempColor[0] = Color[0];
			TempColor[1] = Color[1];
			NewExit.ExitBGColor.a = strtol(TempColor, NULL, 16);
			Color += 2;

			TempColor[0] = Color[0];
			TempColor[1] = Color[1];
			NewExit.ExitBGColor.r = strtol(TempColor, NULL, 16);
			Color += 2;

			TempColor[0] = Color[0];
			TempColor[1] = Color[1];
			NewExit.ExitBGColor.g = strtol(TempColor, NULL, 16);
			Color += 2;

			TempColor[0] = Color[0];
			TempColor[1] = Color[1];
			NewExit.ExitBGColor.b = strtol(TempColor, NULL, 16);
			Color += 2;			
		}
		PropElem = PropElem->NextSiblingElement();
	}

	int WarpIndex = GetWarpIndex(ID);

	if (WarpIndex == -1)
	{
		WarpIndex = Warps.size();
		Warps.push_back(WarpInfo(ID));
	}

	Warps[WarpIndex].Exits.push_back(NewExit);
}

void TMXMap::Render(SDL_Renderer* Renderer, int ScreenX, int ScreenY, int SourceWidth, int SourceHeight)
{
	SimpleSprites.Render(Renderer, RENDER_LAYER_BEHIND_BG);

	if (bPlayingLevel && ThePlayer->IsWarping())
	{
		ThePlayer->Render(Renderer);
	}

	for (int i = 0; i < Layers.size(); i++)
	{
		// Render the item sprites just before the foreground layer
		if (Layers[i] == ForegroundLayer)
		{
			ItemSprites.Render(Renderer, RENDER_LAYER_BEHIND_FG);
			EnemySprites.Render(Renderer, RENDER_LAYER_BEHIND_FG);

			if (bPlayingLevel && !ThePlayer->IsWarping() && ThePlayer->GetRenderLayer() == RENDER_LAYER_BEHIND_FG)
			{				
				ThePlayer->Render(Renderer);
			}
		}
		RenderLayer(Layers[i], Renderer, ScreenX, ScreenY, SourceWidth, SourceHeight);
	}

	if (bRenderCollision)
	{
		RenderLayer(CollisionLayer, Renderer, ScreenX, ScreenY, SourceWidth, SourceHeight);

		for (int i = 0; i < Warps.size(); i++)
		{
			char IDString[4];
			itoa(Warps[i].ID, IDString, 10);

			for (int j = 0; j < Warps[i].Entrances.size(); j++)
			{
				if (InMapWindow(Warps[i].Entrances[j].PosX, Warps[i].Entrances[j].PosY))
				{
					SDL_Rect SrcRect = { 256, 0, 64, 64};
					SDL_Rect DstRect = { (Warps[i].Entrances[j].PosX - ScrollX) * RenderScale, (Warps[i].Entrances[j].PosY - ScrollY) * RenderScale, 64 * RenderScale, 64 * RenderScale };

					SDL_RenderCopyEx(Renderer, MetaTileset->TilesetTexture, &SrcRect, &DstRect, 0, NULL, SDL_FLIP_NONE);
					DrawBitmapText(IDString, DstRect.x, DstRect.y, 32, 32, Renderer, FontShadowedWhite, 1, 1, false);
				}
			}

			for (int j = 0; j < Warps[i].Exits.size(); j++)
			{
				if (InMapWindow(Warps[i].Exits[j].PosX, Warps[i].Exits[j].PosY))
				{
					SDL_Rect SrcRect = { 320, 0, 64, 64 };
					SDL_Rect DstRect = { (Warps[i].Exits[j].PosX - ScrollX) * RenderScale, (Warps[i].Exits[j].PosY - ScrollY) * RenderScale, 64 * RenderScale, 64 * RenderScale };

					SDL_RenderCopyEx(Renderer, MetaTileset->TilesetTexture, &SrcRect, &DstRect, 0, NULL, SDL_FLIP_NONE);
					DrawBitmapText(IDString, DstRect.x, DstRect.y, 32, 32, Renderer, FontShadowedWhite, 1, 1, false);
				}
			}
		}

		for (int i = 0; i < ControlTiles.size(); i++)
		{
			if (InMapWindow(ControlTiles[i]->PosX, ControlTiles[i]->PosY, ControlTiles[i]->Width, ControlTiles[i]->Height))
			{
				char IDString[4];
				itoa(ControlTiles[i]->WarpID, IDString, 10);

				SDL_Rect SrcRect = { 192, 64, 64, 64 };
				SDL_Rect DstRect = { (ControlTiles[i]->PosX - ScrollX) * RenderScale, (ControlTiles[i]->PosY - ScrollY) * RenderScale, ControlTiles[i]->Width * RenderScale, ControlTiles[i]->Height * RenderScale };

				if (DstRect.x < 0)
				{
					DstRect.x = 0;
				}

				if (DstRect.w > TheMap->GetWidth())
				{
					DstRect.w = TheMap->GetWidth();
				}

				if (DstRect.y < 0)
				{
					DstRect.y = 0;
				}

				if (DstRect.h > TheMap->GetHeight())
				{
					DstRect.h = TheMap->GetHeight();
				}

				SDL_RenderCopyEx(Renderer, MetaTileset->TilesetTexture, &SrcRect, &DstRect, 0, NULL, SDL_FLIP_NONE);
				DrawBitmapText(IDString, DstRect.x, DstRect.y, 32, 32, Renderer, FontShadowedWhite, 1, 1, false);
			}
		}
	}

	if (bPlayingLevel)
	{
		ItemSprites.Render(Renderer, RENDER_LAYER_TOP);
		SimpleSprites.Render(Renderer, RENDER_LAYER_TOP);
		EnemySprites.Render(Renderer, RENDER_LAYER_TOP);		

		if (!ThePlayer->IsWarping() && ThePlayer->GetRenderLayer() == RENDER_LAYER_TOP)
		{
			ThePlayer->Render(Renderer);
		}
	}
}

void TMXMap::RenderLayer(const TMXLayer* Layer, SDL_Renderer* Renderer, int ScreenX, int ScreenY, int SourceWidth, int SourceHeight)
{		
	ScrollX = round(ScrollX);
	ScrollY = round(ScrollY);
	int TileOffsetX = (int)(ScrollX) % TilePixelWidth;
	int TileOffsetY = (int)(ScrollY) % TilePixelHeight;

	int StartTileX = (ScrollX) / TilePixelWidth;
	int StartTileY = (ScrollY) / TilePixelHeight;

	int StopTileX = SourceWidth / TilePixelWidth + StartTileX;
	int StopTileY = SourceHeight / TilePixelHeight + StartTileY;

	if (TileOffsetX != 0)
	{		
		StopTileX++;
	}

	if (TileOffsetY != 0)
	{
		StopTileY++;
	}

	for (int y = StartTileY; y < StopTileY && y < Layer->Height; y++)
	{
		if (y < 0)
		{
			continue;
		}

		for (int x = StartTileX; x < StopTileX && x < Layer->Width; x++)
		{
			if (x < 0)
			{
				continue;
			}

			
			TMXTileset* Tileset = GetTilesetFromGID(Layer->TileData[y][x]);			

			if (Tileset != NULL)
			{				
				int TileID = Tileset->GetTileIDFromGID(Layer->TileData[y][x]);

				int SrcX = ((TileID) % Tileset->NumColumns) * Tileset->TilePixelWidth;
				int SrcY = ((TileID) / Tileset->NumColumns) * Tileset->TilePixelHeight;			
				
				int DstX = ScreenX + (x - StartTileX) * Tileset->TilePixelWidth - TileOffsetX;
				int DstY = ScreenY + (y - StartTileY) * Tileset->TilePixelHeight - TileOffsetY;
				int DstWidth = Tileset->TilePixelWidth;
				int DstHeight = Tileset->TilePixelHeight;				

				if (x - StartTileX == 0)
				{
					SrcX += TileOffsetX;					
					DstX += TileOffsetX;
					DstWidth -= TileOffsetX;
				}

				if (x + 1 == StopTileX && TileOffsetX > 0)
				{
					DstWidth = TileOffsetX;
				}

				if (y - StartTileY == 0)
				{
					SrcY += TileOffsetY;
					DstY += TileOffsetY;
					DstHeight -= TileOffsetY;
				}

				if (y + 1 == StopTileY && TileOffsetY > 0)
				{
					DstHeight = TileOffsetY;
				}

				int OffsetIndexX = x % 15;
				int OffsetIndexY = y % 16;

				if (ThePlayer->GetTripLevel() >= 2 && ThePlayer->GetTripLevel() < 4)
				{
					DstX += TileOffsets[OffsetIndexY][OffsetIndexX].OffsetX;
					DstY += TileOffsets[OffsetIndexY][OffsetIndexX].OffsetY;
				}

				/*DstX /= RenderScale;
				DstY /= RenderScale;
				DstWidth /= RenderScale;
				DstHeight /= RenderScale;*/

				SDL_Rect SrcRect = { SrcX, SrcY, DstWidth, DstHeight };
				SDL_Rect DstRect = { DstX * RenderScale, DstY * RenderScale, DstWidth * RenderScale, DstHeight * RenderScale };
				
				SDL_RenderCopyEx(Renderer, Tileset->TilesetTexture, &SrcRect, &DstRect, 0, NULL, SDL_FLIP_NONE);
			}
		}
	}
}

TMXTileset* TMXMap::GetTilesetFromGID(int GID)
{
	TMXTileset* RetVal = NULL;

	if (GID >= 0)
	{
		for (int i = 0; i < Tilesets.size(); i++)
		{
			RetVal = Tilesets[i];

			if (Tilesets[i]->FirstGID <= GID)
			{				
				break;
			}
		}
	}

	return RetVal;
}

void TMXMap::AdjustScrollX(double Amount)
{	
	bool bScrollLocked = Amount > 0 && (ScrollLockFlags & MAP_LOCK_POSITIVE_X);
	bScrollLocked |= Amount < 0 && (ScrollLockFlags & MAP_LOCK_NEGATIVE_X);
	
	if (!bAutoScrollX && !bScrollLocked)
	{
		ScrollX += Amount;

		if (MaxScrollX != 0 && ScrollX > MaxScrollX)
		{
			ScrollX = MaxScrollX;
		}	
	}
}

void TMXMap::AdjustScrollY(double Amount)
{
	bool bScrollLocked = Amount > 0 && (ScrollLockFlags & MAP_LOCK_POSITIVE_Y);
	bScrollLocked |= Amount < 0 && (ScrollLockFlags & MAP_LOCK_NEGATIVE_Y);	

	if (!bAutoScrollY && !bScrollLocked)
	{
		
		ScrollY += Amount;

		if (MaxScrollY != 0 && ScrollY > MaxScrollY)
		{
			ScrollY = MaxScrollY;
		}		
	}
}

void TMXMap::DoWarp(WarpExit Warp)
{
	EnforceWarpControls(Warp);
}

void TMXMap::EnforceWarpControls(WarpExit Warp)
{
	// TODO:
	// bCanGoBackwards = Warp.bCanGoBackwards.

	// TODO:
	// Smooth scroll
	// TODO:
	if (Warp.bChangePositionX)
	{
		ScrollX = Warp.PosX;
	}

	if (Warp.bChangePositionY)
	{
		ScrollY = Warp.PosY;
	}

	// No scroll change is redundent, just set offset to 0 on the tile
	if (!Warp.bNoScrollChange)
	{
		ScrollX += Warp.ScrollOffsetX;
		ScrollY += Warp.ScrollOffsetY;
	}

	ScrollLockFlags = Warp.ScrollLockFlags;
	
	KillY = Warp.KillY;
	MaxScrollX = Warp.MaxScrollX;
	MaxScrollY = Warp.MaxScrollY;
	BrickTilesetID = Warp.BrickTilesetID;

	
	bAutoScrollX = Warp.ScrollVelocityX;
	ScrollVelocityX = Warp.ScrollVelocityX;
	
	bAutoScrollY = Warp.ScrollVelocityY;
	ScrollVelocityY = Warp.ScrollVelocityY;
	
}

void TMXMap::ToggleRenderCollision()
{
	//bRenderCollision = !bRenderCollision;
}

TMXMap::TMXMap()
{	
	ScrollLockFlags = MAP_LOCK_NEGATIVE_X | MAP_LOCK_NEGATIVE_Y | MAP_LOCK_POSITIVE_Y;
	bAutoScrollX = false;
	bAutoScrollY = false;	
	ScrollVelocityX = 0;
	ScrollVelocityY = 0;
	BrickTilesetID = BRICK_TILESET_OVERWORLD;
	MaxScrollX = 0;
	MaxScrollY = 0;
	KillY = -1;
	bPlayingLevel = false;	
	SecondsLeft = 300;
	bRenderCollision = false;
	ScrollX = 0;
	ScrollY = 26;
	/*bLockScrollX = false;
	bLockScrollY = true;*/
}

TMXMap::~TMXMap()
{
	ReleaseAssets();
}

void TMXMap::ReleaseAssets()
{	
	for (int i = 0; i < Tilesets.size(); i++)
	{
		delete Tilesets[i];
	}

	for (int i = 0; i < Layers.size(); i++)
	{
		delete Layers[i];
	}
	ForegroundLayer = NULL;
	MetaTileset = NULL;
	CollisionLayer = NULL;
	Tilesets.clear();
	Layers.clear();
	Warps.clear();

	for (int i = 0; i < ControlTiles.size(); i++)
	{
		delete ControlTiles[i];
	}

	ControlTiles.clear();
}

void TMXMap::StartLevel()
{
	ScrollVelocityX = 0;
	ScrollVelocityY = 0;
	bAutoScrollX = false;
	bAutoScrollY = false;

	bPlayingLevel = true;	
}

void TMXMap::EndLevel()
{
	/*bLockScrollX = false;
	bLockScrollY = true;*/
	bPlayingLevel = false;
	ScrollX = 0;
	ScrollY = 0;
	ReleaseAssets();
}

bool TMXMap::CheckCollision(SDL_Rect Rect, vector<TileInfo>& HitTileLocs, bool bIgnoreBorders, bool bSolidTilesOnly)
{	
	/*if (ThePlayer-> ThePlayer->GetPosX() < TheMap->GetScrollX())
	{
		return false;
	}*/
	// Adjust for scroll
	SDL_Rect MapSpaceRect = { Rect.x + ScrollX, Rect.y + ScrollY, Rect.w, Rect.h };
	int FirstTileX = floor((float)(MapSpaceRect.x) / TilePixelWidth);
	int LastTileX = floor((float)(MapSpaceRect.x + MapSpaceRect.w) / TilePixelWidth);

	int FirstTileY = floor((float)(MapSpaceRect.y) / TilePixelHeight);
	int LastTileY = floor((float)(MapSpaceRect.y + MapSpaceRect.h) / TilePixelHeight);

	//// We are on the left 
	//if (MapSpaceRect.x % 64 < 32)
	//{
	//	LastTileX = FirstTileX;
	//}
	//else
	//{
	//	FirstTileX = LastTileX;
	//}

	if (!bIgnoreBorders && (FirstTileX < 0 || LastTileX >= Width || FirstTileY < 0 || LastTileY >= Height))
	{
		return false;
	}

	if (!bIgnoreBorders && Rect.x <= 0)
	{
		return true;
	}

	bool bCollided = false;

	for (int y = FirstTileY; y <= LastTileY; y++)
	{
		for (int x = FirstTileX; x <= LastTileX; x++)
		{
			if (FirstTileX < 0 || LastTileX >= Width || FirstTileY < 0 || LastTileY >= Height)
			{
				continue;
			}
			SDL_Color CollisionRenderColor;
			SDL_Rect ScreenSpaceCollision = { x * TilePixelWidth - ScrollX, y * TilePixelHeight - ScrollY, TilePixelWidth, TilePixelHeight };
			CollisionRenderColor.a = 255;
			CollisionRenderColor.r = 0;
			CollisionRenderColor.g = 0;
			CollisionRenderColor.b = 255;

			
			if (bRenderCollision)
			{
				SDL_SetRenderDrawBlendMode(GRenderer, SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(GRenderer, CollisionRenderColor.r, CollisionRenderColor.g, CollisionRenderColor.b, 128);

				SDL_RenderDrawRect(GRenderer, &ScreenSpaceCollision);


				SDL_SetRenderDrawBlendMode(GRenderer, SDL_BLENDMODE_NONE);
			}

			HitTileLocs.push_back({ {x, y,}, GetMetaTileType(x, y), ForegroundLayer->TileData[y][x] });

			
			if (!bCollided)
			{
				bCollided = IsCollidableTile(CollisionLayer->TileData[y][x] - MetaTileGID, x, y);
			}
			/*if (bCollided)
			{
				SDL_Color CollisionRenderColor;
				SDL_Rect ScreenSpaceCollision = { x * TilePixelWidth - ScrollX, y * TilePixelHeight - ScrollY, TilePixelWidth, TilePixelHeight };
				CollisionRenderColor.a = 128;
				CollisionRenderColor.r = 255;
				CollisionRenderColor.g = 0;
				CollisionRenderColor.b = 0;

				SDL_SetRenderDrawBlendMode(GRenderer, SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(GRenderer, CollisionRenderColor.r, CollisionRenderColor.g, CollisionRenderColor.b, 128);

				SDL_RenderFillRect(GRenderer, &ScreenSpaceCollision);


				SDL_SetRenderDrawBlendMode(GRenderer, SDL_BLENDMODE_NONE);
				
				return true;
			}*/
		}
	}
	return bCollided;
}

bool TMXMap::CheckCollision(SDL_Rect Rect, bool bIgnoreBorders)
{		
	vector<TileInfo> TouchingTiles;

	return CheckCollision(Rect, TouchingTiles, bIgnoreBorders);
}

eTileMetaType TMXMap::GetMetaTileType(int TileX, int TileY)
{
	// TODO:
	if (TileX < 0 || TileX >= Width || TileY < 0 || TileY >= Height)
	{
		return TILE_NONE;
	}

	return (eTileMetaType)(CollisionLayer->TileData[TileY][TileX] - MetaTileGID);
}

bool TMXMap::IsCollidableTile(int MetaTileID, int TileX, int TileY, SDL_Point TestPoint)
{
	if (MetaTileID == TILE_COLLISION || MetaTileID == TILE_PIPE_ENTRANCE || MetaTileID == TILE_PIPE_EXIT)
	{
		return true;
	}	

	int ForegroundTile = ForegroundLayer->TileData[TileY][TileX];

	if (MetaTileID == TILE_RED_COIN_BLOCK  || MetaTileID == TILE_COIN_BLOCK || MetaTileID == TILE_POWER_UP || MetaTileID == TILE_ONE_UP || MetaTileID == TILE_BREAKABLE || MetaTileID == TILE_MULTI_COIN_BLOCK || MetaTileID == TILE_STAR || MetaTileID == TILE_MAGIC_MUSHROOM || MetaTileID == TILE_DESTROY_WITH_FIRE ||
		MetaTileID == TILE_DESTROY_WITH_FIRE_LEAVE_COLLISION)
	{
		if (ForegroundTile != -1)
		{
			return true;
		}

		// If the secret block is above us...
		if (TestPoint.x >= 0 && TestPoint.y >= 0 && TestPoint.y <= TileY * TilePixelHeight)
		{
			return false;
		}		
	}

	return false;
}

void TMXMap::DoBrickBreak(int TileX, int TileY)
{
	int SpawnX = TileX * 64;
	int SpawnY = TileY * 64;
	eTileMetaType MetaTile = GetMetaTileType(TileX, TileY);

	SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY - 32, -4, MetaTile));
	SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY - 32, 4, MetaTile));
	SimpleSprites.push_back(new BrickBreakSprite(SpawnX, SpawnY + 32, -4, MetaTile));
	SimpleSprites.push_back(new BrickBreakSprite(SpawnX + 32, SpawnY + 32, 4, MetaTile));

	Mix_PlayChannel(CHANNEL_BREAK_BRICK, BreakBrickSound, 0);
	Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);

	SetForegroundTile(TileX, TileY, -1);

	if (MetaTile == TILE_DESTROY_WITH_FIRE_LEAVE_COLLISION)
	{
		SetMetaLayerTile(TileX, TileY, TILE_COLLISION);
	}
	else
	{
		SetMetaLayerTile(TileX, TileY, TILE_NONE);
	}
	
}

bool TMXMap::IsHiddenBlockTile(int ID)
{	

	if (ID == TILE_RED_COIN_BLOCK || ID == TILE_COIN_BLOCK || ID == TILE_POWER_UP || ID == TILE_ONE_UP || ID == TILE_UGLY || ID == TILE_MULTI_COIN_BLOCK || ID == TILE_STAR || ID == TILE_MAGIC_MUSHROOM || ID == TILE_BREAK_ON_TOUCH)
	{
		return true;
	}

	return false;
}

bool TMXMap::IsBreakableBlockTile(int ID)
{

	if (ID == TILE_BREAKABLE || ID == TILE_BREAK_ON_TOUCH)
	{
		return true;
	}

	return false;
}

bool TMXMap::IsCoinTile(int ID)
{

	if (ID == TILE_RED_COIN_BLOCK || ID == TILE_RED_COIN || ID == TILE_COIN || ID == TILE_COIN_BLOCK || ID == TILE_MULTI_COIN_BLOCK)
	{
		return true;
	}

	return false;
}

void TMXMap::SetMetaLayerTile(int TileX, int TileY, eTileMetaType TileType)
{
	// Todo:
	// if within bounds
	CollisionLayer->TileData[TileY][TileX] = TileType + MetaTileGID;
}

void TMXMap::SetForegroundTile(int TileX, int TileY, int TileType)
{
	// Todo:
	// if within bounds
	ForegroundLayer->TileData[TileY][TileX] = TileType + StandardTileGID;
}

void TMXMap::SetBackgroundTile(int TileX, int TileY, int TileType)
{
	// Todo:
	// if within bounds
	BackgroundLayer->TileData[TileY][TileX] = TileType + StandardTileGID;
}

void TMXMap::HandleCollision(int TileX, int TileY, bool bCanBreakBricks)
{
	int MetaTileType = GetMetaTileType(TileX, TileY);	

	if (IsHiddenBlockTile(MetaTileType))
	{
		eItemType ItemTypeToSpawn = ITEM_NONE;		

		if (MetaTileType == TILE_POWER_UP)
		{
			if (ThePlayer->GetPowerUpState() == POWER_NONE)
			{
				ItemTypeToSpawn = ITEM_MUSHROOM;
			}
			else
			{
				ItemTypeToSpawn = ITEM_FLOWER;
			}
		}
		else if (MetaTileType == TILE_ONE_UP)
		{			
			ItemTypeToSpawn = ITEM_ONE_UP;			
		}
		else if (MetaTileType == TILE_STAR)
		{
			ItemTypeToSpawn = ITEM_STAR;
		}
		else if (MetaTileType == TILE_MAGIC_MUSHROOM)
		{
			ItemTypeToSpawn = ITEM_MAGIC_MUSHROOM;
		}

		int TileToReplace = 33;

		// Use underground tiles
		if (BrickTilesetID == 1)
		{
			TileToReplace = 57;
		}

		Sprite* NewBlock = NULL;

		if (MetaTileType == TILE_BREAK_ON_TOUCH)
		{
			DoBrickBreak(TileX, TileY);
		}
		else if (MetaTileType == TILE_MULTI_COIN_BLOCK)
		{
			if (ForegroundLayer->TileData[TileY][TileX] != TILE_NONE)
			{
				int ManagerIndex = GetCoinManagerIndex(TileX, TileY);

				// If there isn't a manager yet, create one
				if (ManagerIndex == -1)
				{
					ManagerIndex = CoinManagers.size();

					CoinManagers.push_back(MultiCoinManager(TileX, TileY, 15));
				}
				CoinManagers[ManagerIndex].CoinsRemaining--;

				// If this block is empty, do the empty block thing
				if (CoinManagers[ManagerIndex].CoinsRemaining <= 0)
				{
					SetMetaLayerTile(TileX, TileY, TILE_COLLISION);
					SetForegroundTile(TileX, TileY, TILE_TRANSPARENT_COLLISION);
					NewBlock = new EmptyBlockSprite(TileX * 64, TileY * 64 - 64, GetEmptyBlockBounceAnimForBrickTileset(), true, TileToReplace, TileX, TileY, ItemTypeToSpawn);
				}
				else
				{
					TileToReplace = ForegroundLayer->TileData[TileY][TileX];

					// TODO: Get the appropriate brick tile number from the map
					if (TileToReplace == -1)
					{												
						if (BrickTilesetID == 1)
						{
							TileToReplace = 54;
						}
						else
						{
							TileToReplace = 33;
						}
					}
					//SetMetaLayerTile(TileX, TileY, TILE_COLLISION);
					SetForegroundTile(TileX, TileY, TILE_TRANSPARENT_COLLISION);
					NewBlock = new EmptyBlockSprite(TileX * 64, TileY * 64 - 64, GetBrickBounceAnimForBrickTileset() , true, TileToReplace, TileX, TileY, ItemTypeToSpawn);
				}
			}
		}
		else
		{

			SetMetaLayerTile(TileX, TileY, TILE_COLLISION);
			SetForegroundTile(TileX, TileY, TILE_TRANSPARENT_COLLISION);
			NewBlock = new EmptyBlockSprite(TileX * 64, TileY * 64 - 64, GetEmptyBlockBounceAnimForBrickTileset(), true, TileToReplace, TileX, TileY, ItemTypeToSpawn);
		}

		if (NewBlock)
		{
			SimpleSprites.push_back(NewBlock);

			if (NewBlock && (MetaTileType == TILE_COIN_BLOCK || MetaTileType == TILE_MULTI_COIN_BLOCK))
			{
				SimpleSprites.push_back(new CoinEffectSprite(TileX * 64, TileY * 64));
				//ThePlayer->AddCoins(1);			
			}

			if (NewBlock && MetaTileType == TILE_RED_COIN_BLOCK)
			{
				SimpleSprites.push_back(new CoinEffectSprite(TileX * 64, TileY * 64, COIN_RED));
				//ThePlayer->AddCoins(1);			
			}
		}		
		
		if (MetaTileType == TILE_UGLY)
		{
			DoUgly();
		}

		Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);		
	}
	else if (IsBreakableBlockTile(MetaTileType) && ForegroundLayer->TileData[TileY][TileX] != TILE_TRANSPARENT_COLLISION)
	{
		if (bCanBreakBricks)
		{
			
			ThePlayer->AddScore(50);
			DoBrickBreak(TileX, TileY);
			Sprite* NewBlock = new EmptyBlockSprite(TileX * 64, TileY * 64 - 64, NULL, false, -1, TileX, TileY);
			SimpleSprites.push_back(NewBlock);
			
		}
		else
		{			
			Sprite* NewBlock = new EmptyBlockSprite(TileX * 64, TileY * 64 - 64, GetBrickBounceAnimForBrickTileset(), true, ForegroundLayer->TileData[TileY][TileX], TileX, TileY);
			SimpleSprites.push_back(NewBlock);			

			SetForegroundTile(TileX, TileY, TILE_TRANSPARENT_COLLISION);
		}
	
		// If the tile above is a coin, grab the coin
		int TileAboveY = TileY - 1;

		if (TileY > 0)
		{
			eTileMetaType MetaTileAbove = GetMetaTileType(TileX, TileAboveY);

			if (MetaTileAbove == TILE_COIN)
			{
				SimpleSprites.push_back(new CoinEffectSprite(TileX * 64, TileAboveY * 64));
				TheMap->SetForegroundTile(TileX, TileAboveY, -1);
				TheMap->SetMetaLayerTile(TileX, TileAboveY, TILE_NONE);
				ThePlayer->AddCoins(1);
				ThePlayer->AddScore(200);
			}
			else if (MetaTileAbove == TILE_RED_COIN)
			{
				SimpleSprites.push_back(new CoinEffectSprite(TileX * 64, TileAboveY * 64, COIN_RED));
				TheMap->SetForegroundTile(TileX, TileAboveY, -1);
				TheMap->SetMetaLayerTile(TileX, TileAboveY, TILE_NONE);
				ThePlayer->AddRedCoins(1, TileX, TileAboveY);
				ThePlayer->AddScore(1000);
			}
		}

		Mix_PlayChannel(CHANNEL_BUMP, BumpSound, 0);
	}


	if (IsCoinTile(MetaTileType))
	{
		if (MetaTileType == TILE_RED_COIN_BLOCK || MetaTileType == TILE_RED_COIN)
		{
			ThePlayer->AddRedCoins(1, TileX, TileY);
			ThePlayer->AddScore(1000);
		}
		else
		{
			ThePlayer->AddCoins(1);
			ThePlayer->AddScore(200);
		}
	}
	
}

void TMXMap::Tick(double DeltaTime)
{
	if (ThePlayer->GetTripLevel() >= 2 && ThePlayer->GetTripLevel() < 4)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int x = 0; x < 15; x++)
			{
				TileOffsets[y][x].OffsetX += TileOffsets[y][x].VelocityX;
				TileOffsets[y][x].OffsetY += TileOffsets[y][x].VelocityY;

				if (abs(TileOffsets[y][x].OffsetX) > 8)
				{
					TileOffsets[y][x].VelocityX *= -1;
				}

				if (abs(TileOffsets[y][x].OffsetY) > 8)
				{
					TileOffsets[y][x].VelocityY *= -1;
				}
			}
		}
	}

	if (!ThePlayer->IsDying() && TheGame->GetGameState() == STATE_LEVEL_PLAY)
	{
		SecondsLeft -= DeltaTime;
	}
	
	for (int i = 0; i < Tilesets.size(); i++)
	{
		Tilesets[i]->TickAnimations();
	}

	for (int i = 0; i < CoinManagers.size(); i++)
	{
		if (CoinManagers[i].Tick())
		{
			CoinManagers.erase(CoinManagers.begin() + i);
			i--;
		}
	}

	// Apply scroll velocities
	if (!ThePlayer->IsWarping() && !ThePlayer->IsChangingSize() && (ScrollVelocityX || ScrollVelocityY))
	{
		AutoScroll();
	}
}

int TMXMap::GetCoinManagerIndex(int TileX, int TileY)
{
	for (int i = 0; i < CoinManagers.size(); i++)
	{
		if (CoinManagers[i].TileX == TileX && CoinManagers[i].TileY == TileY)
		{
			return i;
		}

	}

	return -1;
}

int TMXMap::GetWarpIndex(int ID)
{
	for (int i = 0; i < Warps.size(); i++)
	{
		if (Warps[i].ID == ID)
		{
			return i;
		}

	}

	return -1;
}

bool TMXMap::InMapWindow(int X, int Y)
{
	SDL_Rect MapWindow = TheMap->GetVisibleWindow();

	// To the right of the window
	return (X >= MapWindow.x && X <= MapWindow.x + MapWindow.w &&
		Y >= MapWindow.y && Y <= MapWindow.y + MapWindow.h);
}

bool TMXMap::InMapWindow(int X, int Y, int Width, int Height)
{
	SDL_Rect MapWindow = TheMap->GetVisibleWindow();
	SDL_Rect OtherRect = { X, Y, Width, Height };
	SDL_Rect ResultRect;

	// Intersect is not inclusive, so widen the collision box
	OtherRect.x -= 1;
	OtherRect.w += 2;
	OtherRect.y -= 1;
	OtherRect.h += 2;

	// To the right of the window
	return (SDL_IntersectRect(&MapWindow, &OtherRect, &ResultRect));
}


vector<WarpInfo> TMXMap::GetWarps()
{
	return Warps;
}

SDL_Rect TMXMap::GetVisibleWindow()
{
	SDL_Rect Window;
	
	Window.x = ScrollX;
	Window.w = SCREEN_WIDTH;

	Window.y = ScrollY;
	Window.h = SCREEN_HEIGHT;

	return Window;
}

bool TMXMap::IsDestroyableByFireTile(int ID)
{
	return ID == TILE_DESTROY_WITH_FIRE || ID == TILE_DESTROY_WITH_FIRE_LEAVE_COLLISION;
}

double TMXMap::TradeTimeForPoints(int Amount)
{
	SecondsLeft -= Amount;
	if (SecondsLeft < 0)
	{
		SecondsLeft = 0;
	}
	else
	{
		Mix_PlayChannel(CHANNEL_COIN, CoinSound, 0);
		ThePlayer->AddScore(Amount * 50);
	}
	return SecondsLeft;
}

AnimationResource* TMXMap::GetBrickBounceAnimForBrickTileset()
{


	switch (BrickTilesetID)
	{
	case 1:
		return GResourceManager->BrickBounceUSpriteAnimation;
		break;
	default:
		return GResourceManager->BrickBounceSpriteAnimation;
	}

	return NULL;
}

AnimationResource* TMXMap::GetEmptyBlockBounceAnimForBrickTileset()
{
	

	switch (BrickTilesetID)
	{
	case 1:
		return GResourceManager->EmptyBlockUSpriteAnimation;
		break;
	default:
		return GResourceManager->EmptyBlockSpriteAnimation;
	}

	return NULL;
}

void TMXMap::OnPlayerDie()
{
	StopSAutoScroll();
}

void TMXMap::AutoScroll()
{
	ScrollX += ScrollVelocityX;
	ScrollY += ScrollVelocityY;

	double PlayerPosX = ThePlayer->GetPosX();
	double PlayerPosY = ThePlayer->GetPosY();
	bool bMovedPlayer = false;

	// Push the player along the edge of the screens
	if (bAutoScrollX && ScrollVelocityX > 0)
	{
		if (ThePlayer->GetPosX() - ScrollX < 0)
		{
			bMovedPlayer = true;
			PlayerPosX = ScrollX;
		}
	}

	if (bAutoScrollX && ScrollVelocityX < 0)
	{
		// TODO: Don't use the freakin hardcoded window size....
		if (ThePlayer->GetPosX() + ThePlayer->GetWidth() - ScrollX  > 1024)
		{
			bMovedPlayer = true;
			PlayerPosX = 1024 - ThePlayer->GetWidth() + ScrollX;
		}
	}

	// Check to see if the player got squished
	if (bMovedPlayer)
	{
		ThePlayer->SetPosition(PlayerPosX, PlayerPosY);
		if (CheckCollision(ThePlayer->GetScreenSpaceCollisionRect(), false))
		{
			ThePlayer->BeginDie();
		}
	}
}

void TMXMap::StopSAutoScroll()
{
	ScrollVelocityX = 0;
	ScrollVelocityY = 0;

	bAutoScrollX = false;
	bAutoScrollY = false;
}