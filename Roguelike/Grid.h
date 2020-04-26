#pragma once
#include "modelclass.h"
#include "MapGenerator.h"

enum CellType
{
	EMPTY = 0,
	SOLID = 1,
};

class Grid
{
private:
	int** grid;
	ModelClass cell;
	MapGenerator* mapGen;

public:
	Grid(ID3D11Device*);

	void InitialiseGrid();
	ModelClass GetCell();
	int** GetMatrix();
};