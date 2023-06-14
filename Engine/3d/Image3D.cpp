#include "Image3D.h"
#include "RDirectX.h"
#include "Vertex.h"
#include "LightGroup.h"
#include <Renderer.h>

Image3D::Image3D(TextureHandle texture, Vector2 size, bool forceSize)
{
	mTexture = texture;

	if (forceSize) {
		mSize.x = size.x;
		mSize.y = size.y;
	}
	else {
		scale = size;
		Texture tex = TextureManager::Get(texture);
		mSize.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * scale.x;
		mSize.y = scale.y;
	}

	Init();
}
	
void Image3D::Init()
{
	//頂点データ
	VertexPNU vertices[] = {
		{{ -0.5f * mSize.x, -0.5f * mSize.y, 0.0f }, {}, {0.0f, 1.0f}}, //左下
		{{ -0.5f * mSize.x,  0.5f * mSize.y, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{  0.5f * mSize.x, -0.5f * mSize.y, 0.0f }, {}, {1.0f, 1.0f}}, //右下
		{{  0.5f * mSize.x,  0.5f * mSize.y, 0.0f }, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	VertexPNU::CalcNormalVec(vertices, indices, _countof(indices));

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));
}

void Image3D::TransferBuffer(ViewProjection viewprojection)
{
	material.Transfer(materialBuff.Get());
	transform.Transfer(transformBuff.Get());
	viewProjectionBuff->matrix = viewprojection.matrix;
}

void Image3D::Draw()
{
	std::vector<RootData> rootData{
		{TextureManager::Get(mTexture).gpuHandle},
		{RootDataType::SRBUFFER_CBV, materialBuff.buff },
		{RootDataType::SRBUFFER_CBV, transformBuff.buff },
		{RootDataType::SRBUFFER_CBV, viewProjectionBuff.buff },
		{RootDataType::LIGHT},
	};

	Renderer::DrawCall("Opaque", vertBuff, indexBuff, 6, rootData);
}