#include "pch.h"
#include "Grid.h"

Grid::Grid(ID3D11Device* device)
{
	mapGen = MapGenerator::GetInstance(device);
	cell.InitializeBox(device, 1, 1, 0);
	playerPos = SimpleMath::Vector3();
	grid = new int* [MAP_WIDTH];
	for (int i = 0; i < MAP_WIDTH; i++)
		grid[i] = new int[MAP_HEIGHT];
	InitialiseGrid();
}

void Grid::InitialiseGrid()
{
	//Reset the grid
	for (int i = 0; i < MAP_WIDTH; i++)
		for (int j = 0; j < MAP_HEIGHT; j++)
		{
			grid[i][j] = EMPTY;
		}

	//Rooms
	std::vector<Boundary*>* roomsList = mapGen->GetRoomsList();
	int playerPosIdx = rand() % roomsList->size();
	int treasurePosIdx;
	do { treasurePosIdx = rand() % roomsList->size(); } while (std::abs(playerPosIdx - treasurePosIdx) < 2);
	int iter = 0;
	for (std::vector<Boundary*>::iterator it = roomsList->begin(); it != roomsList->end(); it++)
	{
		Boundary* room = *it;
		int left = room->GetLeft();
		int bottom = room->GetBottom();
		int right = room->GetRight(); 
		int top = room->GetTop();

		for (int i = left; i < right; i++)
			for (int j = bottom; j < top; j++)
				grid[i][j] = SOLID;
		
		if (playerPosIdx == iter)
		{
			grid[(left+right)/2][(top+bottom)/2] = PLAYER;
			playerPos = SimpleMath::Vector3((left + right) / 2 + 0.5f, (top + bottom) / 2 + 0.5f, 0);
		}
		if (treasurePosIdx == iter)
		{
			grid[(left + right) / 2][(top + bottom) / 2] = TREASURE;
			treasurePos = SimpleMath::Vector3((left + right) / 2 + 0.5f, (top + bottom) / 2 + 0.5f, 0);
			debugLine = L"Treasure Grid Cell: " + std::to_wstring((left + right) / 2) + L", " + std::to_wstring((top + bottom) / 2);
		}

		iter++;
	}

	//Corridors
	std::vector<Corridor*>* halls = mapGen->GetHallsList();
	for (std::vector<Corridor*>::iterator it = halls->begin(); it != halls->end(); it++)
	{
		Corridor* corridor = *it;
		int left = corridor->GetLeft();
		int bottom = corridor->GetBottom();
		int right = corridor->GetRight();
		int top = corridor->GetTop();

		for (int i = left; i < right; i++)
			for (int j = bottom; j < top; j++)
				grid[i][j] = SOLID;
	}
}

ModelClass Grid::GetCell()
{
	return cell;
}

int** Grid::GetMatrix()
{
	return grid;
}

SimpleMath::Vector3 Grid::GetPlayerPos()
{
	return playerPos;
}

SimpleMath::Vector3 Grid::GetTreasurePos()
{
	return treasurePos;
}
