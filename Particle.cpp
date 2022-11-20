#include "Particle.h"
#include "Camera.h"
#include "TimeManager.h"

Particle::Particle(RootSignature* root, GraphicsPipeline* pipeline)
	: root(root), pipeline(pipeline) {

	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(VertexInfo));

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
	*vertMap = data;

	//頂点バッファビューの作成
	vertBuffView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	vertBuffView.SizeInBytes = dataSize; //頂点バッファのサイズ
	vertBuffView.StrideInBytes = sizeof(VertexInfo); //頂点一個のサイズ
}

void Particle::Update()
{
	aliveTime -= TimeManager::deltaTime;
	if (aliveTime <= 0) {
		alive = false;
		return;
	}

	float t = min(1, aliveTime / maxAliveTime);
	velocity += accel * TimeManager::deltaFrame;
	data.pos += velocity * TimeManager::deltaFrame;
	data.scale = { scale.x * t, scale.y * t };
	data.color = endcolor * (1 - t) + color * t;

	*vertMap = data;

	Matrix4 mat = Camera::nowCamera->viewProjection.view;
	mat = -mat;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	*billboardBuff.constMap = mat;

	viewProjectionBuff.constMap->matrix = Camera::nowCamera->viewProjection.matrix;
}

void Particle::DrawCommands()
{
	//パイプラインセット
	RDirectX::GetInstance()->cmdList->SetPipelineState(pipeline->ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(root->ptr.Get());

	RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//頂点バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &vertBuffView);

	//定数バッファビューの設定コマンド
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, billboardBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする(背景)
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(texture).gpuHandle);

	//描画コマンド
	RDirectX::GetInstance()->cmdList->DrawInstanced(1, 1, 0, 0); // 全ての頂点を使って描画
	RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}
