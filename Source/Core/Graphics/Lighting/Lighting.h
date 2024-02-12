#pragma once

#include <array>
#include <DirectXMath.h>
#include <string>
#include <Windows.h>

enum class eLightType
{
	Directional,
	Point,
	Spot,
};

static std::string LIGHT_TYPE_NAME[] =
{
	"Directional Light",
	"Point Light",
	"Spot Light"
};

struct DirectionalLightData
{
	DirectX::XMFLOAT3 lightDirection;
	float lightIntensity;
	DirectX::XMFLOAT3 lightColour;
	float ambientLight;
	DirectX::XMMATRIX shadowCamera;
};

struct PointLightData
{
	DirectX::XMFLOAT3 myPosition = {}; // 12
	float myIntensity = {}; // 16
	DirectX::XMFLOAT3 myColour = {}; // 28
	float myRange = {}; // 32
	BOOL isActive = false; // 36
	float padding[3] = {}; // 48
};

struct SpotLightData
{
	DirectX::XMFLOAT3 myPosition = {}; // 12
	float myIntensity = {}; // 16
	DirectX::XMFLOAT3 myDirection = {}; // 28
	float myRange = {}; // 32
	DirectX::XMFLOAT3 myColour = {}; // 44
	float myInnerAngle = {}; // 48
	float myOuterAngle = {}; // 52
	BOOL isActive = false; // 56
	float padding[2] = {}; // 64
};

struct LightConstantBuffer
{
	DirectionalLightData myDirectionalLightData = {};
	std::array<PointLightData, 128u> myPointLightData = {};
	std::array<SpotLightData, 128u> mySpotLightData = {};
	UINT myNumberOfPointLights = {};
	UINT myNumberOfSpotLights = {};
	float padding[2] = {};
};
