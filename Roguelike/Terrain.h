#pragma once

using namespace DirectX;

class Terrain
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
public:
	Terrain();
	~Terrain();

	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Render(ID3D11DeviceContext*);
	bool GenerateRandomHeightMap(ID3D11Device*);
	bool GenerateHeightMap(ID3D11Device*);
	bool Update();
	float* GetWavelength();
	float* GetAmplitude();
	bool Voronoi(ID3D11Device*, int nRegions);
	bool Smoothing(ID3D11Device*);

private:
	bool CalculateNormals();
	void MidPointDisplacement(int, int, int, int, float);
	float Average(float, float);
	float Average(float, float, float, float);
	float Distance(float, float, float, float);
	float jitter(float, float);
	void Shutdown();
	void ShutdownBuffers();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	

private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;

	//Used for collision detection
	SimpleMath::Vector3* loadedVertices;

	int m_vertexCount, m_indexCount;
	float m_frequency, m_amplitude, m_wavelength;
	HeightMapType* m_heightMap;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
};

