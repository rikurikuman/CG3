#include "ModelObj.h"
#include "Light.h"

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	transform.Transfer(transformBuff.constMap);
	viewprojection.Transfer(viewProjectionBuff.constMap);
	//viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::DrawCommands()
{
	for (std::shared_ptr<ModelData> data : model->data) {
		//頂点バッファビューの設定コマンド
		RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &data->vertexBuff.view);

		//インデックスバッファビューの設定コマンド
		RDirectX::GetInstance()->cmdList->IASetIndexBuffer(&data->indexBuff.view);

		//定数バッファビューの設定コマンド
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, data->materialBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());
		RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(4, Light::nowLight->buffer.constBuff->GetGPUVirtualAddress());

		//SRVヒープから必要なテクスチャデータをセットする
		RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(data->material.texture).gpuHandle);

		//描画コマンド
		RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(static_cast<UINT>(data->indices.size()), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
}
