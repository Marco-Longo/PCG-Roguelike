#pragma once
#include "modelclass.h"

class Corridor
{
private:
	ModelClass shape;
	float shapeWidth;
	float shapeHeight;
	float shapeDepth;
	SimpleMath::Vector3 shapePosition;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

public:
public:
	Corridor(ID3D11Device*, float, float, float, float);

	SimpleMath::Vector3 GetPosition();
	ID3D11ShaderResourceView* GetTexture();

	void Render(ID3D11DeviceContext*);
};
