#pragma once
#include "Leaf.h"

#define MAP_WIDTH 50
#define MAP_HEIGHT 30

class MapGenerator
{
private:
	static MapGenerator* instance;

	ID3D11Device* dev;
	int map_width, map_height;
	std::vector<Leaf*> *tree;
	std::vector<Boundary*> *roomsList;

	MapGenerator(ID3D11Device*, int, int);
	void GenerateTree();
	void GenerateRooms();

public:
	static MapGenerator* GetInstance(ID3D11Device*);
	void AddRoom(Boundary*);
	std::vector<Boundary*>* GetRoomsList();
	void ResetLevel();

	std::wstring debugLine;
};