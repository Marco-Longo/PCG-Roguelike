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
	Boundary(ID3D11Device*, float, float, float, float, std::wstring);

	float GetWidth();
	float GetHeight();
	float GetDepth();
	float GetLeft();
	float GetRight();
	float GetTop();
	float GetBottom();
	SimpleMath::Vector3 GetPosition();
	void SetPosition(SimpleMath::Vector3);
	ID3D11ShaderResourceView* GetTexture();

	void Render(ID3D11DeviceContext*);
};