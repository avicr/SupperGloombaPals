#ifndef ASSETRESOURCE_H
#define ASSETRESOURCE_H

#include <string>
#include "Globals.h"

using namespace std;

// Mostly empty class for keeping a list of all assets using polymorphism
class AssetResource 
{
protected:
	string FileName;

public:
	AssetResource(string InFileName);

	void AddReferenceToManager();
};

class TextureResource : public AssetResource
{
public:
	SDL_Texture* Texture;
	SDL_Texture* Texture2;
	bool bCreateOnSecondRenderer;
	SDL_Rect SrcRect;

public:

	TextureResource(string InFileName, bool bCreateSecondTexture) : AssetResource(InFileName) { bCreateOnSecondRenderer = bCreateSecondTexture; }

	TextureResource(const TextureResource &Other) :
		AssetResource(Other.FileName)
	{
		Texture2 = NULL;

		bCreateOnSecondRenderer = Other.bCreateOnSecondRenderer;
		SDL_Log("About to load: %s", Other.FileName.c_str());
		SDL_Surface * Image = SDL_LoadBMP((TEXTURE_PATH + FileName).c_str());
		SDL_Log("Loaded: %d", Image);
		SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0, 0xFF));
		Texture = SDL_CreateTextureFromSurface(GetRenderer(), Image);

		if (bCreateOnSecondRenderer)
		{
			Texture2 = SDL_CreateTextureFromSurface(GetRenderer(2), Image);
		}

		SrcRect = { 0, 0, Image->w, Image->h };
		SDL_FreeSurface(Image);
		AddReferenceToManager();
	}

	~TextureResource()
	{
		SDL_DestroyTexture(Texture);

		if (Texture2)
		{
			SDL_DestroyTexture(Texture2);
		}
	}
};

#endif
