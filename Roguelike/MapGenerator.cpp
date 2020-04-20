#include "pch.h"
#include "MapGenerator.h"

MapGenerator* MapGenerator::instance = 0;

MapGenerator* MapGenerator::GetInstance(ID3D11Device* device)
{
	if (instance == 0)
	{
		instance = new MapGenerator(device);
	}

	return instance;
}

MapGenerator::MapGenerator(ID3D11Device* device)
{
	dev = device;
	roomsList = new std::vector<Boundary*>();
	
	//Generate all the rooms ...
	BSP();
}

void MapGenerator::BSP()
{
//	roomsList->push_back(new Boundary(device, 0.01f, 10.0f, L"whiteBG.dds"));
//	roomsList->push_back(new Boundary(device, 1.0f, 1.0f, 4, 2, L"rock.dds"));
	
	//First step (max size)
	float AsizeX, AsizeY, AcentreX, AcentreY;
	float BsizeX, BsizeY, BcentreX, BcentreY;
	
	float randomX = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
	roomsList->push_back(new Boundary(dev, 0.05f, MAXSIZE, randomX, 0, L"whiteBG.dds"));

	AsizeX = (MAXSIZE / 2.0f) + randomX;
	AsizeY = MAXSIZE;
	AcentreX = -(MAXSIZE / 2.0f) + (AsizeX / 2.0f);
	AcentreY = 0;
	BsizeX = (MAXSIZE / 2.0f) - randomX;
	BsizeY = MAXSIZE;
	BcentreX = (MAXSIZE / 2.0f) - (BsizeX / 2.0f);
	BcentreY = 0;
	
	debugLine = L"Random value: " + std::to_wstring(randomX) + L"\nBoundary A: size(" + std::to_wstring(AsizeX) + L", " + std::to_wstring(AsizeY) + L"); centre(" +
		std::to_wstring(AcentreX) + L", " + std::to_wstring(AcentreY) + L")" + L"\nBoundary B: size(" + std::to_wstring(BsizeX) + L", " +
		std::to_wstring(BsizeY) + L"); centre(" + std::to_wstring(BcentreX) + L", " + std::to_wstring(BcentreY) + L")";

	SplitBoundary(AsizeX, AsizeY, AcentreX, AcentreY);
	SplitBoundary(BsizeX, BsizeY, BcentreX, BcentreY);
}

void MapGenerator::SplitBoundary(float sizeX, float sizeY, float centreX, float centreY)
{
	float AsizeX, AsizeY, AcentreX, AcentreY;
	float BsizeX, BsizeY, BcentreX, BcentreY;
	int direction = rand() % 2;

	//Vertical Split
	if (direction == 0)
	{
		float randomX = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		roomsList->push_back(new Boundary(dev, 0.02f, sizeY, centreX + randomX, centreY, L"whiteBG.dds"));

		AsizeX = (sizeX / 2.0f) + randomX;
		AsizeY = sizeY;
		AcentreX = -(sizeX / 2.0f) + (AsizeX / 2.0f);
		AcentreY = centreY;
		BsizeX = (sizeX / 2.0f) - randomX;
		BsizeY = sizeY;
		BcentreX = (sizeX / 2.0f) - (BsizeX / 2.0f);
		BcentreY = centreY;
	}
	//Horizontal Split
	else
	{
		float randomY = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		roomsList->push_back(new Boundary(dev, sizeX, 0.02f, centreX, centreY + randomY, L"whiteBG.dds"));
		
		AsizeX = sizeX;
		AsizeY = (sizeY / 2.0f) - randomY;
		AcentreX = centreX;
		AcentreY = (sizeY / 2.0f) - (AsizeY / 2.0f);
		BsizeX = sizeX;
		BsizeY = (sizeY / 2.0f) + randomY;
		BcentreX = centreX;
		BcentreY = -(sizeY / 2.0f) + (BsizeY / 2.0f);
	}
}

std::vector<Boundary*>* MapGenerator::GetRoomsList()
{
	return roomsList;
}

void MapGenerator::ResetLevel()
{
	roomsList->clear();
	BSP();
}