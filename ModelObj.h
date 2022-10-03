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

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override;
};

