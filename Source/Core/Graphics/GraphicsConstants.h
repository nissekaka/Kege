#pragma once

#define KAKA_BG_COLOUR {0.1f, 0.2f, 0.3f, 1.0f}

enum
{
	// Common
	PS_CBUFFER_SLOT_COMMON = 4u,
	VS_CBUFFER_SLOT_COMMON = 4u,
	// GBuffer
	PS_GBUFFER_SLOT = 0u,
	PS_TEXTURE_SLOT_GBUFFER_WORLD_POSITION = 0u,
	PS_TEXTURE_SLOT_GBUFFER_COLOUR = 1u,
	PS_TEXTURE_SLOT_GBUFFER_NORMAL = 2u,
	PS_TEXTURE_SLOT_GBUFFER_MATERIAL = 3u,
	PS_TEXTURE_SLOT_GBUFFER_AMBIENT_OCCLUSION = 4u,
	PS_TEXTURE_SLOT_GBUFFER_RSM = 5u,
	// Reflective Shadow Map
	PS_CBUFFER_SLOT_RSM_LIGHT = 0u,
	PS_RSM_SLOT_DIRECTIONAL = 6u,
	PS_RSM_SLOT_SPOT = 9u,
	PS_TEXTURE_SLOT_RSM_WORLD_POSITION = 6u,
	PS_TEXTURE_SLOT_RSM_NORMAL_TEXTURE = 7u,
	PS_TEXTURE_SLOT_RSM_FLUX_TEXTURE = 8u,
	PS_CBUFFER_SLOT_RSM_DIRECTIONAL = 3u,
	PS_CBUFFER_SLOT_RSM_SPOT = 5u,
	// Sky
	PS_TEXTURE_SLOT_DAY_SKY = 12u,
	PS_TEXTURE_SLOT_NIGHT_SKY = 13u,
	// Shadow Map
	PS_TEXTURE_SLOT_SHADOW_MAP_DIRECTIONAL = 14u,
	PS_TEXTURE_SLOT_SHADOW_MAP_SPOT = 15u,
	// Light
	VS_CBUFFER_SLOT_LIGHT = 1u,
	PS_CBUFFER_SLOT_DIRECTIONAL_LIGHT = 1u,
	// TODO Skybox currently needs directional light to be at slot 1
	PS_CBUFFER_SLOT_POINT_LIGHT = 2u,
	PS_CBUFFER_SLOT_SPOT_LIGHT = 2u,
	// Misc
	PS_CBUFFER_SLOT_SHADOW = 7u,
};
