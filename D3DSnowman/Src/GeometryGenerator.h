//***************************************************************************************
// GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines a static class for procedurally generating the geometry of 
// common mathematical objects.
//
// All triangles are generated "outward" facing.  If you want "inward" 
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
//***************************************************************************************

#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include "DXUT.h"
#include <DirectXMath.h>

using namespace DirectX;

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex(){}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv){}
		Vertex(
			float px, float py, float pz, 
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px,py,pz), Normal(nx,ny,nz),
			  TangentU(tx, ty, tz), TexC(u,v){}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	// Creates a box centered at the origin with the given dimensions.
	void CreateBox(float width, float height, float depth, MeshData& meshData);

	// Creates a sphere centered at the origin with the given radius.  The
	// slices and stacks parameters control the degree of tessellation.
	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);

	// Creates a geosphere centered at the origin with the given radius.  The
	// depth controls the level of tessellation.
	void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);

	// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);

	// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	// at the origin with the specified width and depth.
	void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData);

	// Creates a quad covering the screen in NDC coordinates.  This is useful for
	// postprocessing effects.
	void CreateFullscreenQuad(MeshData& meshData);

	// Creates a torus of given diameter, thickness, and tessellation factor.
	void CreateTorus(float diameter, float thickness, UINT tessellation, MeshData& meshData);

private:
	template<typename T>
	T mMin(const T& a, const T& b) { return a < b ? a : b; }

	float AngleFromXY(float x, float y);

	void Subdivide(MeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
};

#endif // GEOMETRYGENERATOR_H