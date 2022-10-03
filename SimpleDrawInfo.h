#pragma once
#include "Vector2.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RConstBuffer.h"
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

class DrawStringInfo : public SimpleDrawInfo {
public:
	Sprite sprite;
};