#include "pch.h"
#include "Boundary.h"

Boundary::Boundary(ID3D11Device* device)
{
	shapeWidth = 6.0f;
	shapeHeight = 4.0f;
	shapeDepth = 0.0f;
	shapePosition = SimpleMath::Vector3(0, 0, 0);

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
	CreateDDSTextureFromFile(device, L"rock.dds", nullptr, texture.ReleaseAndGetAddressOf());
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