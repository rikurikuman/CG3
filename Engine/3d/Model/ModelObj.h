#pragma once
#include "Obj3D.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "RConstBuffer.h"

class ModelObj : public Obj3D
{
public:
	Model* model = nullptr;

	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	ModelObj() {};
	ModelObj(Model* model) : model(model) {};
	ModelObj(ModelHandle handle) : model(ModelManager::Get(handle)) {};

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

