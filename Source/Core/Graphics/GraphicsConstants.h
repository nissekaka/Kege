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
	PS_RSM_SLOT = 6u,
	PS_TEXTURE_SLOT_RSM_WORLD_POSITION = 6u,
	PS_TEXTURE_SLOT_RSM_NORMAL_TEXTURE = 7u,
	PS_TEXTURE_SLOT_RSM_FLUX_TEXTURE = 8u,
	PS_CBUFFER_SLOT_RSM = 2u,
	// Sky
	PS_TEXTURE_SLOT_DAY_SKY = 12u,
	PS_TEXTURE_SLOT_NIGHT_SKY = 13u,
	// Shadow Map
	PS_TEXTURE_SLOT_SHADOW_MAP = 14u,

	// Misc
	PS_CBUFFER_SLOT_SHADOW = 7u,
};