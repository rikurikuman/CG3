#include "IndexBuffer.h"
#include "RDirectX.h"

IndexBuffer::IndexBuffer(uint32_t* list, uint32_t size)
{
	Init(list, size);
}

IndexBuffer::IndexBuffer(std::vector<uint32_t> list)
{
	Init(list);
}

void IndexBuffer::Init(uint32_t* list, uint32_t size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(uint32_t) * size);

	//インデックスバッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//インデックスバッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);

	//GPU上のバッファに対応した仮想メモリを取得
	//インデックスバッファをマッピング
	uint32_t* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (uint32_t i = 0; i < size; i++)
	{
		indexMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	view.BufferLocation = buff->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = dataSize;
}

void IndexBuffer::Init(std::vector<uint32_t> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(uint32_t) * list.size());

	//インデックスバッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//インデックスバッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);

	//GPU上のバッファに対応した仮想メモリを取得
	//インデックスバッファをマッピング
	uint32_t* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (uint32_t i = 0; i < list.size(); i++)
	{
		indexMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	view.BufferLocation = buff->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = dataSize;
}
