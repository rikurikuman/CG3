#include "PolygonParticle.h"
#include "TimeManager.h"
#include "Camera.h"

PolygonParticle::PolygonParticle(RootSignature* root, GraphicsPipeline* pipeline)
	: root(root), pipeline(pipeline) {

	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * 3);

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = RDirectX::GetInstance()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//頂点バッファビューの作成
	vertBuffView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	vertBuffView.SizeInBytes = dataSize; //頂点バッファのサイズ
	vertBuffView.StrideInBytes = sizeof(VertexPNU); //頂点一個のサイズ
}

void PolygonParticle::Update()
{
	aliveTime -= TimeManager::deltaTime;
	if (aliveTime <= 0) {
		alive = false;
		return;
	}

	float t = min(1, aliveTime / maxAliveTime);
	velocity += accel * TimeManager::deltaFrame;
	transform.position += velocity;
	transform.UpdateMatrix();
	
	for (int i = 0; i < 3; i++) {
		vertMap[i] = vertices[i];
	}

	transformBuff.constMap->matrix = transform.matrix;
	material.Transfer(materialBuff.constMap);
	viewProjectionBuff.constMap->matrix = Camera::nowCamera->viewProjection.matrix;
}

void PolygonParticle::DrawCommands()
{
	//パイプラインセット
	RDirectX::GetInstance()->cmdList->SetPipelineState(pipeline->ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(root->ptr.Get());

	//頂点バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &vertBuffView);

	//定数バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする(背景)
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(material.texture).gpuHandle);

	//描画コマンド
	RDirectX::GetInstance()->cmdList->DrawInstanced(3, 1, 0, 0); // 全ての頂点を使って描画

}
