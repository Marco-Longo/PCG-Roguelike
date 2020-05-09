#pragma once
#include "modelclass.h"

class Player
{
private:
	ModelClass shape;
	float shapeWidth;
	float shapeHeight;
	float shapeDepth;
	float speed;
	SimpleMath::Vector3 shapePosition;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureFlipped;
	bool flipped;

public:
	Player(ID3D11Device*, SimpleMath::Vector3);

	float GetWidth();
	float GetHeight();
	float GetDepth();
	float GetSpeed();
	SimpleMath::Vector3 GetPosition();
	float* GetPositionX();
	float* GetPositionY();
	void SetPosition(SimpleMath::Vector3);
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetTextureFlipped();
	void FlipSprite(bool);
	bool IsFlipped();

	void CheckBoundaries(int**);
	void Render(ID3D11DeviceContext*);
};