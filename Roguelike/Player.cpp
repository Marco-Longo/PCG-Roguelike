#include "pch.h"
#include "Player.h"

Player::Player(ID3D11Device * device)
{
	shapeWidth = 0.2f;
	shapeHeight = 0.3f;
	shapeDepth = 0.0f;
	speed = 3.0f;
	shapePosition = SimpleMath::Vector3(0.5f, 0.5f, 0);

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
	CreateDDSTextureFromFile(device, L"greenShape.dds", nullptr, texture.ReleaseAndGetAddressOf());
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

void Player::CheckBoundaries(int** grid)
{
	float minX1, maxX1, minY1, maxY1;
	float minX2, maxX2, minY2, maxY2;

	//Player Rectangle
	minX1 = shapePosition.x - (shapeWidth / 2.0f);
	maxX1 = minX1 + shapeWidth;
	minY1 = shapePosition.y - (shapeHeight / 2.0f);
	maxY1 = minY1 + shapeHeight;

	//Current Grid Cell
	minX2 = (int)minX1;
	maxX2 = minX2 + 1;
	minY2 = (int)minY1;
	maxY2 = minY2 + 1;

	if (maxY1 > maxY2) //Right border
		shapePosition.y = maxY2 - (shapeHeight / 2.0f);
	if (minY1 < minY2) //Left border
		shapePosition.y = minY2 + (shapeHeight / 2.0f);
	if (maxX1 > maxX2) //Top border
		shapePosition.x = maxX2 - (shapeWidth / 2.0f);
	if (minX1 < minX2) //Bottom border
		shapePosition.x = minX2 + (shapeWidth / 2.0f);
}

void Player::Render(ID3D11DeviceContext* context)
{
	shape.Render(context);
}