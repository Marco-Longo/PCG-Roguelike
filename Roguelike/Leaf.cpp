#include "pch.h"
#include "Leaf.h"

Leaf::Leaf(ID3D11Device* device, int _x, int _y, int _width, int _height)
{
	dev = device;
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	room = nullptr;
	halls = nullptr;
	leftChild = rightChild = nullptr;
}

bool Leaf::SplitLeaf()
{
	//The leaf is already been split
	if (leftChild != nullptr || rightChild != nullptr)
		return false;

	//Decide whether to split horizontally or vertically
	bool splitH = ((float)rand() / RAND_MAX) > 0.5f;
	if ((width > height) && (width / height >= 1.25f))
		splitH = false;
	else if ((height > width) && (height / width >= 1.25f))
		splitH = true;

	//Determine the maximum height/width
	int max = (splitH ? height : width) - MIN_LEAF_SIZE;
	if (max <= MIN_LEAF_SIZE) //the area is too small to be split
		return false;

	//Generate the random point where to split the leaf
	int split = randBetween(MIN_LEAF_SIZE, max);
	if (splitH) //Split Horizontally
	{
		leftChild = new Leaf(dev, x, y, width, split);
		rightChild = new Leaf(dev, x, y + split, width, height - split);
	}
	else //Split Vertically
	{
		leftChild = new Leaf(dev, x, y, split, height);
		rightChild = new Leaf(dev, x + split, y, width - split, height);
	}
	//Split was successful
	return true;
}

void Leaf::CreateRooms()
{
	//This function generates all the rooms and hallways for this Leaf and all of its children.
	if (leftChild != nullptr || rightChild != nullptr)
	{
		//This leaf has been split, so go into the children leafs
		if (leftChild != nullptr)
		{
			leftChild->CreateRooms();
		}
		if (rightChild != nullptr)
		{
			rightChild->CreateRooms();
		}
		//If there are both left and right children in this leaf, create an hallway between them
		if (leftChild != nullptr && rightChild != nullptr)
		{
			CreateHall(leftChild->GetRoomRecursive(), rightChild->GetRoomRecursive());
		}
	}
	else
	{
		//This Leaf is ready to have a room
		SimpleMath::Vector2 roomSize;
		SimpleMath::Vector2 roomPos;
		//The room can be between (min_size x min_size) units to the size of the leaf - 2.
		int min_size = MIN_LEAF_SIZE - 2;
		roomSize = SimpleMath::Vector2(randBetween(min_size, width - 2), randBetween(min_size, height - 2));
		//Place the room within the Leaf, but don't put it right against the side of the Leaf (that would merge rooms together)
		roomPos = SimpleMath::Vector2(randBetween(1, width - roomSize.x - 1), randBetween(1, height - roomSize.y - 1));
		room = new Boundary(dev, x + roomPos.x, y + roomPos.y, roomSize.x, roomSize.y, L"whiteBG.dds");
	}
}

void Leaf::CreateHall(Boundary* l, Boundary* r)
{
	//Connect these two rooms together with hallways. Draw a straight line, or a pair of lines to make a right-angle to connect them.
	halls = new std::vector<Corridor*>();

	SimpleMath::Vector2 point1 = SimpleMath::Vector2(randBetween(l->GetLeft() + 1, l->GetRight() - 2), randBetween(l->GetTop() + 1, l->GetBottom() - 2));
	SimpleMath::Vector2 point2 = SimpleMath::Vector2(randBetween(r->GetLeft() + 1, r->GetRight() - 2), randBetween(r->GetTop() + 1, r->GetBottom() - 2));
	float w = point2.x - point1.x;
	float h = point2.y - point1.y;

	if (w < 0)
	{
		if (h < 0)
		{
			if (((float)rand() / RAND_MAX) < 0.5f)
			{
				halls->push_back(new Corridor(dev, point2.x, point1.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point2.x, point2.y, 1, std::abs(h)));
			}
			else
			{
				halls->push_back(new Corridor(dev, point2.x, point2.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point1.x, point2.y, 1, std::abs(h)));
			}
		}
		else if (h > 0)
		{
			if (((float)rand() / RAND_MAX) < 0.5f)
			{
				halls->push_back(new Corridor(dev, point2.x, point1.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point2.x, point1.y, 1, std::abs(h)));
			}
			else
			{
				halls->push_back(new Corridor(dev, point2.x, point2.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point1.x, point1.y, 1, std::abs(h)));
			}
		}
		else
		{
			halls->push_back(new Corridor(dev, point2.x, point2.y, std::abs(w), 1));
		}
	}
	else if (w > 0)
	{
		if (h < 0)
		{
			if (((float)rand() / RAND_MAX) < 0.5f)
			{
				halls->push_back(new Corridor(dev, point1.x, point2.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point1.x, point2.y, 1, std::abs(h)));
			}
			else
			{
				halls->push_back(new Corridor(dev, point1.x, point1.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point2.x, point2.y, 1, std::abs(h)));
			}
		}
		else if (h > 0)
		{
			if (((float)rand() / RAND_MAX) < 0.5f)
			{
				halls->push_back(new Corridor(dev, point1.x, point1.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point2.x, point1.y, 1, std::abs(h)));
			}
			else
			{
				halls->push_back(new Corridor(dev, point1.x, point2.y, std::abs(w), 1));
				halls->push_back(new Corridor(dev, point1.x, point1.y, 1, std::abs(h)));
			}
		}
		else
		{
			halls->push_back(new Corridor(dev, point1.x, point1.y, std::abs(w), 1));
		}
	}
	else
	{
		if (h < 0)
		{
			halls->push_back(new Corridor(dev, point2.x, point2.y, 1, std::abs(h)));
		}
		else if (h > 0)
		{
			halls->push_back(new Corridor(dev, point1.x, point1.y, 1, std::abs(h)));
		}
	}
}

int Leaf::GetWidth()
{
	return width;
}

int Leaf::GetHeight()
{
	return height;
}

Leaf* Leaf::GetLeftChild()
{
	return leftChild;
}

Leaf* Leaf::GetRightChild()
{
	return rightChild;
}

void Leaf::SetLeftChild(Leaf* leaf)
{
	leftChild = leaf;
}

void Leaf::SetRightChild(Leaf* leaf)
{
	rightChild = leaf;
}

Boundary* Leaf::GetRoom()
{
	return room;
}

Boundary* Leaf::GetRoomRecursive()
{
	//Iterate all the way through the leafs to find a room, if one exists
	if (room != nullptr)
		return room;
	else
	{
		Boundary* lRoom = nullptr;
		Boundary* rRoom = nullptr;

		if (leftChild != nullptr)
			lRoom = leftChild->GetRoomRecursive();
		if (rightChild != nullptr)
			rRoom = rightChild->GetRoomRecursive();

		if (lRoom == nullptr && rRoom == nullptr)
			return nullptr;
		else if (rRoom == nullptr)
			return lRoom;
		else if (lRoom == nullptr)
			return rRoom;
		else if (((float)rand() / RAND_MAX) > 0.5f)
			return lRoom;
		else
			return rRoom;
	}
}

std::vector<Corridor*>* Leaf::GetHallsList()
{
	return halls;
}

void Leaf::SetRoom(Boundary* bound)
{
	room = bound;
}

int Leaf::randBetween(int min, int max)
{
	return (rand() % (max - min + 1) + min);
}