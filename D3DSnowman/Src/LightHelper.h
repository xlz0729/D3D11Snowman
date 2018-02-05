//***************************************************************************************
// LightHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper classes for lighting.
//***************************************************************************************

#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include "DXUT.h"

using namespace DirectX;

// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.

struct DirectionalLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.

	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	XM_CONSTEXPR DirectionalLight(XMFLOAT4 x, XMFLOAT4 y, XMFLOAT4 z, XMFLOAT3 w) : Ambient(x), Diffuse(y), Specular(z), Direction(w), Pad(0.0f) {}

	DirectionalLight& operator= (const DirectionalLight& dl) 
	{ 
		Ambient = dl.Ambient; Diffuse = dl.Diffuse; Specular = dl.Specular;
		Direction = dl.Direction; Pad = dl.Pad; return *this; 
	}
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	// Packed into 4D vector: (Direction, Spot)
	XMFLOAT3 Direction;
	float Spot;

	// Packed into 4D vector: (Att, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct Material
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;

	Material() { ZeroMemory(this, sizeof(this)); }
	XM_CONSTEXPR Material(XMFLOAT4 x, XMFLOAT4 y, XMFLOAT4 z, XMFLOAT4 w) : Ambient(x), Diffuse(y), Specular(z), Reflect(w) {}

	Material& operator= (const Material& dl)
	{
		Ambient = dl.Ambient; Diffuse = dl.Diffuse; Specular = dl.Specular;
		Reflect = dl.Reflect; return *this;
	}
};

#endif // LIGHTHELPER_H