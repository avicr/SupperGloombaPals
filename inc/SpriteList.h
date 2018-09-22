#ifndef SPRITELIST_H
#define SPRITELIST_H

#include "../inc/Sprite.h"
#include "../inc/EnemySprite.h"
#include <vector>

template <class T>
class SpriteList : public vector <T>
{
public:
	void Tick(double DeltaTime);
	void CheckCollision();
	void Render(SDL_Renderer *Renderer, eRenderLayer RenderLayer);
	void DeleteAll();
	void DeletePendingActors();
};


template<class T>
void SpriteList<T>::Tick(double DeltaTime)
{
	for (int i = size() - 1; i >= 0; i--)
	{
		(*this)[i]->Tick(DeltaTime);
	}

	DeletePendingActors();
}

template<class T>
void SpriteList<T>::DeletePendingActors()
{
	for (int i = 0; i < size(); i++)
	{
		if ((*this)[i]->GetPendingDelete())
		{
			delete ((*this)[i]);
			(*this).erase(begin() + i);
			i--;
		}
	}
}

template<class T>
void SpriteList<T>::Render(SDL_Renderer *Renderer, eRenderLayer RenderLayer)
{
	for (int i = size() - 1; i >= 0; i--)
	{
		if ((*this)[i]->GetRenderLayer() == RenderLayer)
		{
			(*this)[i]->Render(Renderer);

			/*SDL_Rect CollisionRect = Mexicans[i]->GetScreenSpaceCollisionRect();
			SDL_SetRenderDrawColor(GRenderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(GRenderer, &CollisionRect);*/
		}
	}
}

template<class T>
void SpriteList<T>::DeleteAll()
{
	if (size() > 0)
	{
		for (int i = size() - 1; i >= 0; i--)
		{
			delete (*this)[i];
			(*this).erase(begin() + i);
		}
		resize(0);
	}
}

#endif

