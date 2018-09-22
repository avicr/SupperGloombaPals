#include "../inc/Animation.h"

Frame::Frame()
{
	Texture = NULL;
	FrameTime = 0;
	SrcRect.x = 0;
	SrcRect.y = 0;
	SrcRect.w = 0;
	SrcRect.h = 0;
}

Frame::Frame(SDL_Texture* InTexture, SDL_Rect InSrcRect, double InFrameTime) :
	Texture(InTexture), SrcRect(InSrcRect), FrameTime(InFrameTime)
{

}

Frame *AnimationResource::GetFrame(int FrameNum)
{
	if (FrameNum >= 0 && FrameNum < Frames.size())
	{
		return Frames[FrameNum];
	}

	return NULL;
}

int AnimationResource::GetFrameCount()
{
	return Frames.size();
}

SDL_Texture *Frame::GetTexture()
{
	return Texture;
}

SDL_Rect Frame::GetSrcRect()
{
	return SrcRect;
}

double Frame::GetFrameTime()
{
	return FrameTime;
}

AnimationResource::AnimationResource() :
	AssetResource("")
{
	bUseAbsoluteFrameCount = false;
}

AnimationResource::AnimationResource(const AnimationResource &Other) :
	AssetResource(Other.FileName)
{	
	bUseAbsoluteFrameCount = false;
	AddReferenceToManager();
}

AnimationResource::~AnimationResource()
{
	for (int i = 0; i < Frames.size(); i++)
	{
		delete Frames[i];
	}
}

bool AnimationResource::UseAbsoluteFrameCount()
{
	return bUseAbsoluteFrameCount;
}