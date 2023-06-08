#pragma once
#include "Obj3D.h"
#include "SRConstBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector2.h"
#include "SRVertexBuffer.h"
#include "SRIndexBuffer.h"

class Image3D final : public Obj3D
{
private:
	TextureHandle mTexture;
	Vector2 scale = { 1, 1 };
	Vector2 mSize = { 1, 1 };
	bool mForceSize = false;

public:
	Material material;
	//TransformはObj3Dにある

	SRVertexBuffer vertBuff;
	SRIndexBuffer indexBuff;
	SRConstBuffer<MaterialBuffer> materialBuff;
	SRConstBuffer<TransformBuffer> transformBuff;
	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Image3D() {};

	Image3D(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	void SetTexture(TextureHandle texture) {
		mTexture = texture;
		if (!mForceSize) {
			Texture tex = TextureManager::Get(texture);
			mSize.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * mSize.x;
		}
	}

	TextureHandle GetTexture() {
		return mTexture;
	}

	void SetSize(Vector2 size, bool forceSize = false) {
		mForceSize = forceSize;
		if (forceSize) {
			mSize = size;
		}
		else {
			scale = size;
			Texture tex = TextureManager::Get(mTexture);
			mSize.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * scale.x;
			mSize.y = scale.y;
		}
	}

	Vector2 GetSize() {
		return mSize;
	}

	//初期化処理
	void Init();

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求をRendererへ
	void Draw() override;
};

