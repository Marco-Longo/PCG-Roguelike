#pragma once
#include "modelclass.h";

class Player
{
private:
	ModelClass shape;
	float shapeWidth;
	float shapeHeight;
	float shapeDepth;
	SimpleMath::Vector3 shapePosition;
	SimpleMath::Vector3 shapeCentre;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

public:
	Player(ID3D11Device*);

	float GetWidth();
	float GetHeight();
	float GetDepth();
	SimpleMath::Vector3 GetCentre();
	void SetCentre(SimpleMath::Vector3);
};