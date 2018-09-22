#ifndef ASSETRESOURCE_H
#define ASSETRESOURCE_H

#include <string>

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

#endif
