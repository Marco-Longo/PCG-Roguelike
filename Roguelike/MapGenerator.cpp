#include "pch.h"
#include "MapGenerator.h"

MapGenerator* MapGenerator::instance = 0;

MapGenerator* MapGenerator::GetInstance(ID3D11Device* device)
{
	if (instance == 0)
	{
		instance = new MapGenerator(device, MAP_WIDTH, MAP_HEIGHT);
	}

	return instance;
}

MapGenerator::MapGenerator(ID3D11Device* device, int width, int height)
{
	dev = device;
	map_width = width;
	map_height = height;
	roomsList = new std::vector<Boundary*>();
	hallsList = new std::vector<Corridor*>();
	tree = new std::vector<Leaf*>();
	GenerateTree();
	GenerateRooms();
	GenerateHalls();
}

void MapGenerator::GenerateTree()
{
	tree->clear();
	const int MAX_LEAF_SIZE = 16;

	//Create the root of the tree
	Leaf* root = new Leaf(dev, 0, 0, map_width, map_height);
	tree->push_back(root);

	bool didSplit = true;
	while (didSplit)
	{
		didSplit = false;
		Leaf* leaf = nullptr;
		for (std::vector<Leaf*>::iterator it = tree->begin(); it != tree->end(); it++)
		{
			Leaf* l = *it;
			//If the leaf has not been split yet...
			if (l->GetLeftChild() == nullptr && l->GetRightChild() == nullptr)
			{
				float r = ((float)rand() / RAND_MAX);
				//... and it is still too wide or high (or 75% chance)
				if ((l->GetWidth() > MAX_LEAF_SIZE) || (l->GetHeight() > MAX_LEAF_SIZE) || r < 0.75f)
				{
					leaf = l; //This is the leaf we want to split
					break;
				}
			}
		}

		if (leaf) //If a splittable leaf was found...
		{
			if (leaf->SplitLeaf()) //Split the leaf
			{
				//If the split was successful, add the new leaves to the tree
				if (leaf->GetLeftChild())
					tree->push_back(leaf->GetLeftChild());
				if (leaf->GetRightChild())
					tree->push_back(leaf->GetRightChild());
				didSplit = true;
			}
		}
	}
	debugLine = L"Tree size: " + std::to_wstring(tree->size());
	
	if (tree->size() < 11) //Check if the generated dungeon is big enough
		GenerateTree();
	else //Iterate through each Leaf and create a room in each one
		root->CreateRooms();
}

void MapGenerator::GenerateRooms()
{
	for (std::vector<Leaf*>::iterator it = tree->begin(); it != tree->end(); it++)
	{
		Leaf* l = *it;
		if (l->GetRoom() != nullptr)
			AddRoom(l->GetRoom());
	}
}

void MapGenerator::GenerateHalls()
{
	for (std::vector<Leaf*>::iterator it = tree->begin(); it != tree->end(); it++)
	{
		Leaf* l = *it;
		std::vector<Corridor*>* halls = l->GetHallsList();
		if (halls != nullptr)
		{
			for (std::vector<Corridor*>::iterator iter = halls->begin(); iter != halls->end(); iter++)
				AddCorridor(*iter);
		}
	}
}

void MapGenerator::AddRoom(Boundary* bound)
{
	roomsList->push_back(bound);
}

void MapGenerator::AddCorridor(Corridor* hall)
{
	hallsList->push_back(hall);
}

std::vector<Boundary*>* MapGenerator::GetRoomsList()
{
	return roomsList;
}

std::vector<Corridor*>* MapGenerator::GetHallsList()
{
	return hallsList;
}

void MapGenerator::ResetLevel()
{
	//Clear the current dungeon
	roomsList->clear();
	hallsList->clear();
	//Generate new level
	GenerateTree();
	GenerateRooms();
	GenerateHalls();
}