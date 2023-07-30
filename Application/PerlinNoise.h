#pragma once
#include <Renderer.h>
#include <RenderTarget.h>
#include <SRConstBuffer.h>

class PerlinNoise
{
public:
	struct NoiseData {
		float t;
	};

	NoiseData noiseData{};

	PerlinNoise();

	void Render();

	TextureHandle GetTexHandle() {
		if (!renderTex) return "";
		return renderTex->mTexHandle;
	}

private:
	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<NoiseData> noiseDataBuff;

	RenderTexture* renderTex = nullptr;
	static RootSignature& GetRootSignature();
	static GraphicsPipeline& GetGraphicsPipeline();
};

