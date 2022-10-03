#pragma once
#include "Obj3D.h"
#include "RConstBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector2.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Image3D final : public Obj3D
{
private:
	TextureHandle texture;
	Vector2 scale = { 1, 1 };
	Vector2 size = { 1, 1 };
	bool forceSize = false;

public:
	Material material;
	//Transform��Obj3D�ɂ���

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Image3D() {};

	Image3D(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	void SetTexture(TextureHandle tex) {
		texture = tex;
		if (!forceSize) {
			Texture tex = TextureManager::Get(texture);
			this->size.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * size.x;
			this->size.y = size.y;
		}
	}

	TextureHandle GetTexture() {
		return texture;
	}

	void SetSize(Vector2 size, bool forceSize = false) {
		this->forceSize = forceSize;
		if (forceSize) {
			this->size = size;
		}
		else {
			scale = size;
			Texture tex = TextureManager::Get(texture);
			this->size.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * scale.x;
			this->size.y = scale.y;
		}
	}

	Vector2 GetSize() {
		return size;
	}

	//����������
	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

