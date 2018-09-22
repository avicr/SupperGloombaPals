#include "../inc/AssetResource.h"
#include "../inc/ResourceManager.h"

AssetResource::AssetResource(string InFileName) :
	FileName(InFileName)
{

}

void AssetResource::AddReferenceToManager()
{
	ResourceManager::AllResources.push_back(this);
}