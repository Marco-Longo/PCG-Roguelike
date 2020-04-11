// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture : register(t0);
Texture2D shaderTexture1 : register(t1);
Texture2D shaderTexture2 : register(t2);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
	float4	grassColor;
	float4	slopeColor;
	float4	rockColor;
	float	slope;
	float	alpha;
    float3	lightDir;
    float	lightIntensity;
    float4	color;
	float4	heightColor;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);

	/*
	//Task 1
	//Sample the pixel color from the textures using the sampler at this texture coordinate location.
	grassColor = shaderTexture.Sample(SampleType, input.tex);
	slopeColor = shaderTexture1.Sample(SampleType, input.tex);
	rockColor = shaderTexture2.Sample(SampleType, input.tex);

	//Calculate the slope at the current point
	slope = 1.0f - input.normal.y;
	//Set the blending value
	if (slope < 0.2f)
	{
		alpha = slope / 0.2f;
		textureColor = lerp(grassColor, slopeColor, alpha);
	}
	else if (slope >= 0.2f && slope < 0.7f)
	{
		alpha = (slope - 0.2f) * (1.0f / (0.7f - 0.2f));
		textureColor = lerp(slopeColor, rockColor, alpha);
	}
	else
	{
		textureColor = rockColor;
	}
	color = color * textureColor;
	
	//Task 2
	if (input.position3D.y > -0.5f && input.position3D.y <= 0.5f)
		heightColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (input.position3D.y > 0.5f && input.position3D.y <= 1.5f)
		heightColor = float4(0.54f, 0.27f, 0.07f, 1.0f);
	else if (input.position3D.y > 1.5f)
		heightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	else
		heightColor = float4(0.76f, 0.69f, 0.5f, 1.0f);
	color = color * heightColor;

	//Voronoi Only (5 colors)
	if (input.position3D.y <= 0.0f)
		heightColor = float4(0.76f, 0.69f, 0.5f, 1.0f);
	else if (input.position3D.y > 0.0f && input.position3D.y <= 0.5f)
		heightColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (input.position3D.y > 0.5f && input.position3D.y <= 1.0f)
		heightColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (input.position3D.y > 1.0f && input.position3D.y <= 1.6f)
		heightColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (input.position3D.y > 1.6f && input.position3D.y <= 2.0f)
		heightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	color = color * heightColor;
	*/

	float4 newColor = float4(input.tex.x, input.tex.y, input.tex.x, 1.0f);
	return color * newColor;
}
