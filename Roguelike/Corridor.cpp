#include "pch.h"
#include "Corridor.h"

Corridor::Corridor(ID3D11Device* device, float x, float y, float width, float height)
{
	shapeWidth = width;
	shapeHeight = height;
	shapeDepth = 0.0f;
	//The BSP algorithm will need to use the bottom left corner as the origin of the rectangle
	shapePosition = SimpleMath::Vector3(x + shapeWidth / 2, y + shapeHeight / 2, 0);

	shape.InitializeBox(device, shapeWidth, shapeHeight, shapeDepth);
	CreateDDSTextureFromFile(device, L"blackBG.dds", nullptr, texture.ReleaseAndGetAddressOf());
}

SimpleMath::Vector3 Corridor::GetPosition()
{
	return shapePosition;
}

ID3D11ShaderResourceView* Corridor::GetTexture()
{
	return texture.Get();
}

void Corridor::Render(ID3D11DeviceContext* context)
{
	shape.Render(context);
}