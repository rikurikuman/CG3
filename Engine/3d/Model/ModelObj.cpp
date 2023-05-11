#include "ModelObj.h"
#include "LightGroup.h"
#include <Renderer.h>

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	for (std::shared_ptr<ModelData> data : model->data) {
		Material mat = data->material;
		mat.color.r *= tuneMaterial.color.r;
		mat.color.g *= tuneMaterial.color.g;
		mat.color.b *= tuneMaterial.color.b;
		mat.color.a *= tuneMaterial.color.a;
		mat.ambient *= tuneMaterial.ambient;
		mat.diffuse *= tuneMaterial.diffuse;
		mat.specular *= tuneMaterial.specular;
		mat.Transfer(data->materialBuff.constMap);
	}

	transform.Transfer(transformBuff.Get());
	viewprojection.Transfer(viewProjectionBuff.Get());
	//viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::Draw()
{
	for (std::shared_ptr<ModelData> data : model->data) {
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->material.texture).gpuHandle },
			{ RootDataType::CBV, data->materialBuff.constBuff->GetGPUVirtualAddress() },
			{ RootDataType::SRBUFFER_CBV, transformBuff.buff },
			{ RootDataType::SRBUFFER_CBV, viewProjectionBuff.buff },
			{ RootDataType::LIGHT }
		};

		std::string stage = "Opaque";
		if (data->material.color.a < 1.0f || tuneMaterial.color.a < 1.0f) stage = "Transparent";
		Renderer::DrawCall(stage, &data->vertexBuff.view, &data->indexBuff.view, static_cast<UINT>(data->indices.size()), rootData);
	}
}