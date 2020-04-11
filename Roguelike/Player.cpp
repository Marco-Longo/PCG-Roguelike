#include "pch.h"
#include "Player.h"

Player::Player(ID3D11Device * device)
{
	shapeWidth = 0.5f;
	shapeHeight = 0.0f;
	shapeDepth = 0.5f;
	shapePosition = SimpleMath::Vector3(0, 0, 0);
	shapeCentre = SimpleMath::Vector3(0, 0, 0);

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
}