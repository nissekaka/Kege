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
	DirectX::XMFLOAT3 lightDirection; // 12
	float lightIntensity; // 16
	DirectX::XMFLOAT3 lightColour; // 28
	float ambientLight; // 32
	DirectX::XMMATRIX shadowCamera; // 96
	DirectX::XMMATRIX spotShadowCamera; // 160
};

struct PointLightData
{
	DirectX::XMFLOAT3 position = {}; // 12
	float intensity = {}; // 16
	DirectX::XMFLOAT3 colour = {}; // 28
	float radius = {}; // 32
	BOOL isActive = false; // 36
	float padding[3] = {}; // 48
};

struct SpotLightData
{
	DirectX::XMFLOAT3 position = {}; // 12
	float intensity = {}; // 16
	DirectX::XMFLOAT3 direction = {}; // 28
	float range = {}; // 32
	DirectX::XMFLOAT3 colour = {}; // 44
	float innerAngle = {}; // 48
	float outerAngle = {}; // 52
	BOOL isActive = false; // 56
	DirectX::XMMATRIX shadowCamera; // 120
	float padding[2] = {}; // 128
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
