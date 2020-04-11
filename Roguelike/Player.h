#pragma once
#include "modelclass.h";
#include "Shader.h";

class Player
{
private:
	ModelClass shape;
	float shapeWidth;
	float shapeHeight;
	float shapeDepth;
	float speed;
	SimpleMath::Vector3 shapePosition;
	SimpleMath::Vector3 shapeCentre;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

public:
	Player(ID3D11Device*);

	float GetWidth();
	float GetHeight();
	float GetDepth();
	float GetSpeed();
	SimpleMath::Vector3 GetCentre();
	void SetCentre(SimpleMath::Vector3);
	SimpleMath::Vector3 GetPosition();
	float* GetPositionX();
	float* GetPositionY();
	void SetPosition(SimpleMath::Vector3);
	ID3D11ShaderResourceView* GetTexture();

	void CheckBoundaries(ModelClass);
	void Render(ID3D11DeviceContext*);
};