#include "pch.h"
#include "Treasure.h"

Treasure::Treasure(ID3D11Device* device, SimpleMath::Vector3 pos)
{
	shapeWidth = 0.7;
	shapeHeight = 0.7;
	shapeDepth = 0.0f;
	shapePosition = pos;
	active = true;

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
	CreateDDSTextureFromFile(device, L"treasure.dds", nullptr, texture.ReleaseAndGetAddressOf());
}

float Treasure::GetWidth()
{
	return shapeWidth;
}

float Treasure::GetHeight()
{
	return shapeHeight;
}

SimpleMath::Vector3 Treasure::GetPosition()
{
	return shapePosition;
}

void Treasure::SetPosition(SimpleMath::Vector3 pos)
{
	shapePosition = pos;
}

void Treasure::SetActive(bool b)
{
	active = b;
}

bool Treasure::IsActive()
{
	return active;
}

ID3D11ShaderResourceView* Treasure::GetTexture()
{
	return texture.Get();
}

void Treasure::Render(ID3D11DeviceContext* context)
{
	shape.Render(context);
}
