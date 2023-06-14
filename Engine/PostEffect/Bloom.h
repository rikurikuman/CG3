#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>
class Bloom
{
public:
	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;

	Bloom();

	void Draw();

protected:
	static RootSignature& GetRootSignature();
	static GraphicsPipeline& GetGraphicsPipelineA(); //–¾‚é‚³’Šo
	static GraphicsPipeline& GetGraphicsPipelineB(); //ƒKƒEƒX‚Ú‚©‚µ
	static GraphicsPipeline& GetGraphicsPipelineC(); //‰ÁZ‡¬
};