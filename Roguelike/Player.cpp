#include "pch.h"
#include "Player.h"

Player::Player(ID3D11Device * device, SimpleMath::Vector3 pos)
{
	shapeWidth = 0.4f;
	shapeHeight = 0.5f;
	shapeDepth = 0.0f;
	speed = 3.0f;
	shapePosition = pos;

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
	CreateDDSTextureFromFile(device, L"character.dds", nullptr, texture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"characterFlipped.dds", nullptr, textureFlipped.ReleaseAndGetAddressOf());
	flipped = false;
}

float Player::GetWidth()
{
	return shapeWidth;
}

float Player::GetHeight()
{
	return shapeHeight;
}

float Player::GetDepth()
{
	return shapeDepth;
}

float Player::GetSpeed()
{
	return speed;
}

SimpleMath::Vector3 Player::GetPosition()
{
	return shapePosition;
}

//Only used by Imgui
float* Player::GetPositionX()
{
	return &shapePosition.x;
}

//Only used by Imgui
float* Player::GetPositionY()
{
	return &shapePosition.y;
}

void Player::SetPosition(SimpleMath::Vector3 pos)
{
	shapePosition = pos;
}

ID3D11ShaderResourceView* Player::GetTexture()
{
	return texture.Get();
}

ID3D11ShaderResourceView* Player::GetTextureFlipped()
{
	return textureFlipped.Get();
}

void Player::FlipSprite(bool b)
{
	flipped = b;
}

bool Player::IsFlipped()
{
	return flipped;
}

void Player::CheckBoundaries(int** grid)
{
	//MinX = x, MaxX = y, MinY = z, MaxY = w
	SimpleMath::Vector4 playerRect;
	SimpleMath::Vector4 currCellRect;
	SimpleMath::Vector4 neighbours[4];

	//Player Rectangle
	playerRect.x = shapePosition.x - (shapeWidth / 2.0f);
	playerRect.y = playerRect.x + shapeWidth;
	playerRect.z = shapePosition.y - (shapeHeight / 2.0f);
	playerRect.w = playerRect.z + shapeHeight;

	//Current Grid Cell
	currCellRect.x = (int)shapePosition.x; //i
	currCellRect.y = currCellRect.x + 1.0f;
	currCellRect.z = (int)shapePosition.y; //j
	currCellRect.w = currCellRect.z + 1.0f;

	//Neighbours: MinX = x (i), MaxX = y, MinY = z (j), MaxY = w
	neighbours[0] = SimpleMath::Vector4(currCellRect.x, currCellRect.x + 1, currCellRect.z + 1, currCellRect.z + 2); //Top
	neighbours[1] = SimpleMath::Vector4(currCellRect.x - 1, currCellRect.x, currCellRect.z, currCellRect.z + 1); //Left
	neighbours[2] = SimpleMath::Vector4(currCellRect.x, currCellRect.x + 1, currCellRect.z - 1, currCellRect.z); //Bottom
	neighbours[3] = SimpleMath::Vector4(currCellRect.x + 1, currCellRect.x + 2, currCellRect.z, currCellRect.z + 1); //Right

	//Top
	if (grid[(int)neighbours[0].x][(int)neighbours[0].z] == 0) //Top neighbour cell is empty
	{
		if (playerRect.w > currCellRect.w) //Top border
			shapePosition.y = currCellRect.w - (shapeHeight / 2.0f);
	}
	//Left
	if (grid[(int)neighbours[1].x][(int)neighbours[1].z] == 0) //Left neighbour cell is empty
	{
		if (playerRect.x < currCellRect.x) //Left border
			shapePosition.x = currCellRect.x + (shapeWidth / 2.0f);
	}
	//Bottom
	if (grid[(int)neighbours[2].x][(int)neighbours[2].z] == 0) //Bottom neighbour cell is empty
	{
		if (playerRect.z < currCellRect.z) //Bottom border
			shapePosition.y = currCellRect.z + (shapeHeight / 2.0f);
	}
	//Right
	if (grid[(int)neighbours[3].x][(int)neighbours[3].z] == 0) //Right neighbour cell is empty
	{
		if (playerRect.y > currCellRect.y) //Right border
			shapePosition.x = currCellRect.y - (shapeWidth / 2.0f);
	}
}

void Player::Render(ID3D11DeviceContext* context)
{
	shape.Render(context);
}