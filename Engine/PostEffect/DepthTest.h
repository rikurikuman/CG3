#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>
#include <RenderTarget.h>

class DepthTest
{
public:
	struct BlurSetting {
		float angle = 0;
		float pickRange = 0.06f;
	};

	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<BlurSetting> mConstBuff;

	BlurSetting mSetting{};

	DepthTest();

	void Draw();

protected:
	RenderTargetTexture* tex;

	static RootSignature& GetRootSignatureA();
	static GraphicsPipeline& GetGraphicsPipelineA();
	static GraphicsPipeline& GetGraphicsPipelineB();
};