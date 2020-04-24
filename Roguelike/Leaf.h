#pragma once
#include "Boundary.h"
#include "Corridor.h"
#include <vector>

#define MIN_LEAF_SIZE 6

class Leaf
{
private:
	ID3D11Device* dev;
	int x, y, width, height;
	Leaf *leftChild, *rightChild;
	Boundary* room;
	std::vector<Corridor*>* halls;

	int randBetween(int, int);

public:
	Leaf(ID3D11Device*, int, int, int, int);
	bool SplitLeaf();
	void CreateRooms();
	void CreateHall(Boundary*, Boundary*);

	int GetWidth();
	int GetHeight();
	Leaf* GetLeftChild();
	Leaf* GetRightChild();
	void SetLeftChild(Leaf*);
	void SetRightChild(Leaf*);
	Boundary* GetRoom();
	Boundary* GetRoomRecursive();
	std::vector<Corridor*>* GetHallsList();
	void SetRoom(Boundary*);
};