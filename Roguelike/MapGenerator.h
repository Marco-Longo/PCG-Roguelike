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
	std::vector<Corridor*> *hallsList;

	MapGenerator(ID3D11Device*, int, int);
	void GenerateTree();
	void GenerateRooms();
	void GenerateHalls();

public:
	static MapGenerator* GetInstance(ID3D11Device*);
	void AddRoom(Boundary*);
	void AddCorridor(Corridor*);
	std::vector<Boundary*>* GetRoomsList();
	std::vector<Corridor*>* GetHallsList();
	void ResetLevel();

	std::wstring debugLine;
};