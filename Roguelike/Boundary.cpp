#include "pch.h"
#include "Boundary.h"

Boundary::Boundary(ID3D11Device* device, float x, float y, float width, float height, std::wstring filename)
{
	shapeWidth = width;
	shapeHeight = height;
	shapeDepth = 0.0f;
	//The BSP algorithm will need to use the bottom left corner as the origin of the rectangle
	shapePosition = SimpleMath::Vector3(x + shapeWidth/2, y + shapeHeight/2, 0);

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
	CreateDDSTextureFromFile(device, filename.c_str(), nullptr, texture.ReleaseAndGetAddressOf());
}

float Boundary::GetWidth()
{
	return shapeWidth;
}

float Boundary::GetHeight()
{
	return shapeHeight;
}

float Boundary::GetDepth()
{
	return shapeDepth;
}

float Boundary::GetLeft()
{
	return shapePosition.x - (shapeWidth / 2.0f);
}

float Boundary::GetRight()
{
	return shapePosition.x + (shapeWidth / 2.0f);
}

float Boundary::GetTop()
{
	return shapePosition.y + (shapeHeight / 2.0f);
}

float Boundary::GetBottom()
{
	return shapePosition.y - (shapeHeight / 2.0f);
}

SimpleMath::Vector3 Boundary::GetPosition()
{
	return shapePosition;
}

void Boundary::SetPosition(SimpleMath::Vector3 pos)
{
	shapePosition = pos;
}

ID3D11ShaderResourceView* Boundary::GetTexture()
{
	return texture.Get();
}

void Boundary::Render(ID3D11DeviceContext* context)
{
	shape.Render(context);
}