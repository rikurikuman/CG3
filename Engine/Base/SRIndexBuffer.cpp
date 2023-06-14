#include "SRIndexBuffer.h"

std::recursive_mutex SRIndexBuffer::mutex;

SRIndexBuffer::SRIndexBuffer(uint32_t* list, uint32_t size)
{
	Init(list, size);
}

SRIndexBuffer::SRIndexBuffer(std::vector<uint32_t> list)
{
	Init(list);
}

void SRIndexBuffer::Init(uint32_t* list, uint32_t size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = std::make_shared<IndexBufferData>();
		data->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(uint32_t) * size);

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	uint32_t* vertMap = reinterpret_cast<uint32_t*>(data->buff.Get());
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->indexCount = size;
}

void SRIndexBuffer::Init(std::vector<uint32_t> list)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = std::make_shared<IndexBufferData>();
		data->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(uint32_t) * list.size());

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	uint32_t* vertMap = reinterpret_cast<uint32_t*>(data->buff.Get());
	for (uint32_t i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->indexCount = static_cast<uint32_t>(list.size());
}

D3D12_INDEX_BUFFER_VIEW SRIndexBuffer::GetIndexView()
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);
	if (data == nullptr || data->buff.GetRegionPtr() == nullptr) {
		return D3D12_INDEX_BUFFER_VIEW();
	}
	D3D12_INDEX_BUFFER_VIEW view{};
	view.BufferLocation = data->buff.GetGPUVirtualAddress();
	view.SizeInBytes = data->dataSize; //頂点バッファのサイズ
	view.Format = DXGI_FORMAT_R32_UINT;
	return view;
}

uint32_t SRIndexBuffer::GetIndexCount()
{
	std::lock_guard<std::recursive_mutex> lock2(mutex);
	if (data == nullptr) {
		return 0;
	}
	return data->indexCount;
}
