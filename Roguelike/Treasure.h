#pragma once
#include "modelclass.h"

class Treasure
{
private:
	ModelClass shape;
	float shapeWidth;
	float shapeHeight;
	float shapeDepth;
	SimpleMath::Vector3 shapePosition;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	bool active;

public:
	Treasure(ID3D11Device*, SimpleMath::Vector3);

	float GetWidth();
	float GetHeight();
	SimpleMath::Vector3 GetPosition();
	void SetPosition(SimpleMath::Vector3);
	void SetActive(bool);
	bool IsActive();
	ID3D11ShaderResourceView* GetTexture();
	void Render(ID3D11DeviceContext*);
};