#pragma once
#include "modelclass.h";

class Boundary
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
	Boundary(ID3D11Device*);

	float GetWidth();
	float GetHeight();
	float GetDepth();
	SimpleMath::Vector3 GetPosition();
	void SetPosition(SimpleMath::Vector3);
	ID3D11ShaderResourceView* GetTexture();

	void Render(ID3D11DeviceContext*);
};

