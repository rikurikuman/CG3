#include "ModelObj.h"
#include "LightGroup.h"

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
