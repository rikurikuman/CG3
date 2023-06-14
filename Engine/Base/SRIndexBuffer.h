#pragma once
#include <vector>
#include <SRBuffer.h>

class SRIndexBuffer {
private:
	struct IndexBufferData {
		size_t count = 0;
		uint32_t dataSize = 0;
		uint32_t indexCount = 0;
		SRBufferPtr buff;
	};

	static std::recursive_mutex mutex;

public:
	SRIndexBuffer() {};

	~SRIndexBuffer() {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
		std::lock_guard<std::recursive_mutex> lock2(mutex);
		if (data != nullptr) {
			data->count--;
			if (data->count == 0) {
				SRBufferAllocator::Free(data->buff);
			}
		}
	}

	//ÉRÉsÅ[ëŒçÙ
	SRIndexBuffer(const SRIndexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (data != nullptr) {
			data->count--;
			if (data->count == 0) {
				SRBufferAllocator::Free(data->buff);
			}
		}
		data = o.data;
		if (data != nullptr) data->count++;
	}

	SRIndexBuffer& operator=(const SRIndexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (this != &o) {
			if (data != nullptr) {
				data->count--;
				if (data->count == 0) {
					SRBufferAllocator::Free(data->buff);
				}
			}
			data = o.data;
			if (data != nullptr) data->count++;
		}
		return *this;
	}

	SRIndexBuffer(uint32_t* list, uint32_t size);
	SRIndexBuffer(std::vector<uint32_t> list);

	void Init(uint32_t* list, uint32_t size);
	void Init(std::vector<uint32_t> list);

	bool IsValid() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return this->data != nullptr && this->data->buff.GetRegionPtr() != nullptr && this->data->buff.GetRegionPtr()->region != nullptr;
	}

	operator bool() {
		return IsValid();
	}

	D3D12_INDEX_BUFFER_VIEW GetIndexView();
	uint32_t GetIndexCount();

private:
	std::shared_ptr<IndexBufferData> data = nullptr;
};