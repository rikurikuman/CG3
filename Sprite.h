#pragma once
#include <d3d12.h>
#include "Vector2.h"
#include "Texture.h"
#include "Material.h"
#include "Transform.h"
#include "RConstBuffer.h"
#include "ViewProjection.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Sprite
{
public:
	TextureHandle texture;
	Material material;
	Transform transform;
	Vector2 size = { 0, 0 };
	Vector2 anchor = { 0.5f, 0.5f };

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Sprite() {}
	Sprite(TextureHandle texture, Vector2 anchor = { 0.5f, 0.5f });

	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer();

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands();
};

class SpriteManager
{
public:
	static SpriteManager* GetInstance() {
		static SpriteManager instance;
		return &instance;
	}

	RootSignature GetRootSignature() {
		return rootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return pipelineState;
	}

private:
	SpriteManager() {
		Init();
	};
	~SpriteManager() = default;
	SpriteManager(const SpriteManager& a) {};
	SpriteManager& operator=(const SpriteManager&) { return *this; }

	void Init();

	RootSignature rootSignature;
	GraphicsPipeline pipelineState;
};

