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

	transform.Transfer(transformBuff.constMap);
	viewprojection.Transfer(viewProjectionBuff.constMap);
	//viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::Draw()
{
	for (std::shared_ptr<ModelData> data : model->data) {
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->material.texture).gpuHandle },
			{ RootDataType::CBV, data->materialBuff.constBuff->GetGPUVirtualAddress() },
			{ RootDataType::CBV, transformBuff.constBuff->GetGPUVirtualAddress() },
			{ RootDataType::CBV, viewProjectionBuff.constBuff->GetGPUVirtualAddress() },
			{ RootDataType::LIGHT }
		};

		std::string stage = "Opaque";
		if (data->material.color.a < 1.0f || tuneMaterial.color.a < 1.0f) stage = "Transparent";
		Renderer::DrawCall(stage, &data->vertexBuff.view, &data->indexBuff.view, static_cast<UINT>(data->indices.size()), rootData);
	}
}

void ModelObj::DrawCommands()
{
	for (std::shared_ptr<ModelData> data : model->data) {
		//頂点バッファビューの設定コマンド
		RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &data->vertexBuff.view);

		//インデックスバッファビューの設定コマンド
		RDirectX::GetCommandList()->IASetIndexBuffer(&data->indexBuff.view);

		//定数バッファビューの設定コマンド
		RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(1, data->materialBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(4, LightGroup::nowLight->buffer.constBuff->GetGPUVirtualAddress());

		//SRVヒープから必要なテクスチャデータをセットする
		RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureManager::Get(data->material.texture).gpuHandle);

		//描画コマンド
		RDirectX::GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(data->indices.size()), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
}
