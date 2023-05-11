#pragma once
#include "Obj3D.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "SRConstBuffer.h"

class ModelObj : public Obj3D
{
public:
	Model* model = nullptr;
	Material tuneMaterial = Material(true);

	SRConstBuffer<TransformBuffer> transformBuff;
	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	ModelObj() {};
	ModelObj(Model* model) : model(model) {};
	ModelObj(ModelHandle handle) : model(ModelManager::Get(handle)) {};

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw();
};

