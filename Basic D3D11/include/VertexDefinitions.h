#ifndef VERTEX_DEFINITIONS_H
#define VERTEX_DEFINITIONS_H
#include "..\SimpleMath.h"

// *************************************************************************************
// Structures
// *************************************************************************************
struct SimpleVertex
{
	DirectX::SimpleMath::Vector3 Pos;
	DirectX::SimpleMath::Vector4 Color;
};

struct ConstantBuffer
{
	DirectX::SimpleMath::Matrix mWorld;
	DirectX::SimpleMath::Matrix mView;
	DirectX::SimpleMath::Matrix mProjection;
};


#endif