#pragma once
#include "Obj3D.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "RConstBuffer.h"
#include "SRConstBuffer.h"
#include <map>
#include <Renderer.h>

class ModelObj : public Obj3D
{
public:
	Model* mModel = nullptr;
	Material mTuneMaterial = Material(true);

	std::unordered_map<std::string, SRConstBuffer<MaterialBuffer>> mMaterialBuffMap;
	SRConstBuffer<TransformBuffer> mTransformBuff;
	SRConstBuffer<ViewProjectionBuffer> mViewProjectionBuff;

	ModelObj() {};
	virtual ~ModelObj() override {}
	ModelObj(Model* model) : mModel(model) {};
	ModelObj(ModelHandle handle) : mModel(ModelManager::Get(handle)) {};

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw() override;
};

