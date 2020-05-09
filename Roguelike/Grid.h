#pragma once
#include "modelclass.h"
#include "MapGenerator.h"

enum CellType
{
	EMPTY = 0,
	SOLID = 1,
	PLAYER = 2,
	TREASURE = 3,
};

class Grid
{
private:
	int** grid;
	ModelClass cell;
	MapGenerator* mapGen;
	SimpleMath::Vector3 playerPos;
	SimpleMath::Vector3 treasurePos;

public:
	Grid(ID3D11Device*);

	void InitialiseGrid();
	ModelClass GetCell();
	int** GetMatrix();
	SimpleMath::Vector3 GetPlayerPos();
	SimpleMath::Vector3 GetTreasurePos();

	std::wstring debugLine;
};