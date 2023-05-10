#pragma once
#include "Vector2.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RConstBuffer.h"
#include "SRConstBuffer.h"
#include "Material.h"
#include "ViewProjection.h"
#include "Sprite.h"

class SimpleDrawInfo
{
};

class DrawBoxInfo : public SimpleDrawInfo {
public:
	Vector2 start;
	Vector2 end;

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	RConstBuffer<Color> colorBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	GraphicsPipeline pipelineState;
};

class DrawCircleInfo : public SimpleDrawInfo {
public:
	Vector2 pos;
	Color color;
	float r = 0;

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	UINT indexCount = 0;
	D3D12_VERTEX_BUFFER_VIEW vertView{};
	D3D12_INDEX_BUFFER_VIEW indexView{};
	SRConstBuffer<Color> colorBuff;

	GraphicsPipeline pipelineState;
};

class DrawStringInfo : public SimpleDrawInfo {
public:
	Sprite sprite;
};