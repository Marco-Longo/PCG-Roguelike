#pragma once
#include "Boundary.h"
#include <vector>

#define MAXSIZE 10

class MapGenerator
{
private:
	static MapGenerator* instance;
	ID3D11Device* dev;
	std::vector<Boundary*> *roomsList;

	MapGenerator(ID3D11Device*);
	void BSP();
	void SplitBoundary(float, float, float, float);
	void SplitBoundaries(float, float, float, float, float, float, float, float);

public:
	static MapGenerator* GetInstance(ID3D11Device*);
	std::vector<Boundary*>* GetRoomsList();
	void ResetLevel();

	std::wstring debugLine;
};