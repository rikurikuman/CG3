#pragma once
#include "Vertex.h"
#include <SRBuffer.h>

class SRVertexBuffer
{
private:
	class VertexBufferData {
	public:
		size_t count = 0;
		UINT dataSize = 0;
		UINT strideInBytes = 0;
		SRBufferPtr buff;
	};

	static std::recursive_mutex mutex;

public:
	SRVertexBuffer() {};

	~SRVertexBuffer() {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
		std::lock_guard<std::recursive_mutex> lock2(mutex);
		if (data != nullptr) {
			data->count--;
			if (data->count == 0) {
				SRBufferAllocator::Free(data->buff);
				delete data;
			}
		}
	}

	//コピー対策
	SRVertexBuffer(const SRVertexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (data != nullptr) {
			data->count--;
			if (data->count == 0) {
				SRBufferAllocator::Free(data->buff);
				delete data;
			}
		}
		data = o.data;
		if(data != nullptr) data->count++;
	}

	SRVertexBuffer& operator=(const SRVertexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (this != &o) {
			if (data != nullptr) {
				data->count--;
				if (data->count == 0) {
					SRBufferAllocator::Free(data->buff);
					delete data;
				}
			}
			data = o.data;
			if (data != nullptr) data->count++;
		}
		return *this;
	}

	bool IsValid() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return this->data != nullptr && this->data->buff.GetRegionPtr() != nullptr && this->data->buff.GetRegionPtr()->region != nullptr;
	}

	operator bool() {
		return IsValid();
	}

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	SRVertexBuffer(VertexP* list, unsigned int size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	SRVertexBuffer(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	SRVertexBuffer(VertexPNU* list, unsigned int size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	SRVertexBuffer(std::vector<VertexPNU> list);

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	void Init(VertexP* list, unsigned int size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	void Init(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	void Init(VertexPNU* list, unsigned int size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	void Init(std::vector<VertexPNU> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを更新する
	void Update(VertexPNU* list, unsigned int size);

	D3D12_VERTEX_BUFFER_VIEW GetVertView();

private:
	VertexBufferData* data = nullptr;
};

