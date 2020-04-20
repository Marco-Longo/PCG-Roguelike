#include "pch.h"
#include "Boundary.h"

Boundary::Boundary(ID3D11Device* device, float width, float height, std::wstring filename)
{
	shapeWidth = width;
	shapeHeight = height;
	shapeDepth = 0.0f;
	shapePosition = SimpleMath::Vector3(0, 0, 0);

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
	CreateDDSTextureFromFile(device, filename.c_str(), nullptr, texture.ReleaseAndGetAddressOf());
}

Boundary::Boundary(ID3D11Device* device, float width, float height, float x, float y, std::wstring filename)
{
	shapeWidth = width;
	shapeHeight = height;
	shapeDepth = 0.0f;
	shapePosition = SimpleMath::Vector3(x, y, 0);

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