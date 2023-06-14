#pragma once
#include "Vertex.h"
#include <SRBuffer.h>

class SRVertexBuffer
{
private:
	struct VertexBufferData {
		size_t count = 0;
		uint32_t dataSize = 0;
		uint32_t strideInBytes = 0;
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
			}
		}
	}

	//�R�s�[�΍�
	SRVertexBuffer(const SRVertexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (data != nullptr) {
			data->count--;
			if (data->count == 0) {
				SRBufferAllocator::Free(data->buff);
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

	//Vertex(Pos)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	SRVertexBuffer(VertexP* list, uint32_t size);
	//Vertex(Pos)��vector�Œ��_�o�b�t�@�����
	SRVertexBuffer(std::vector<VertexP> list);

	//Vertex(PosNormalUv)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	SRVertexBuffer(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)��vector�Œ��_�o�b�t�@�����
	SRVertexBuffer(std::vector<VertexPNU> list);

	//Vertex(Pos)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	void Init(VertexP* list, uint32_t size);
	//Vertex(Pos)��vector�Œ��_�o�b�t�@�����
	void Init(std::vector<VertexP> list);

	//Vertex(PosNormalUv)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	void Init(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)��vector�Œ��_�o�b�t�@�����
	void Init(std::vector<VertexPNU> list);

	//Vertex(PosNormalUv)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@���X�V����
	void Update(VertexPNU* list, uint32_t size);

	//�C�ӂ̒��_�f�[�^�̔z��Ƃ��̑傫���ō��
	template<class T>
	void Init(T* list, uint32_t size) {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
		std::lock_guard<std::recursive_mutex> lock2(mutex);

		if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
			SRBufferAllocator::Free(data->buff);
		}
		else {
			data = std::make_shared<VertexBufferData>();
			data->count++;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * size);

		data->buff = SRBufferAllocator::Alloc(dataSize, 1);

		T* vertMap = reinterpret_cast<T*>(data->buff.Get());
		for (uint32_t i = 0; i < size; i++) {
			vertMap[i] = list[i];
		}

		data->dataSize = dataSize;
		data->strideInBytes = sizeof(T);
	}

	//�C�ӂ̒��_�f�[�^��vector�ō��
	template<class T>
	void Init(std::vector<T> list) {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
		std::lock_guard<std::recursive_mutex> lock2(mutex);

		if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
			SRBufferAllocator::Free(data->buff);
		}
		else {
			data = std::make_shared<VertexBufferData>();
			data->count++;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * list.size());

		data->buff = SRBufferAllocator::Alloc(dataSize, 1);

		T* vertMap = reinterpret_cast<T*>(data->buff.Get());
		for (uint32_t i = 0; i < list.size(); i++) {
			vertMap[i] = list[i];
		}

		data->dataSize = dataSize;
		data->strideInBytes = sizeof(T);
	}

	//�C�ӂ̒��_�f�[�^�̔z��Ƃ��̑傫���ōX�V
	template<class T>
	void Update(T* list, uint32_t size) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (data == nullptr || data->buff.GetRegionPtr() == nullptr) {
			Init(list, size);
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * size);

		if (data->dataSize != dataSize || data->strideInBytes != sizeof(T)) {
			Init(list, size);
			return;
		}

		T* vertMap = reinterpret_cast<T*>(data->buff.Get());
		for (uint32_t i = 0; i < size; i++) {
			vertMap[i] = list[i];
		}

		data->dataSize = dataSize;
		data->strideInBytes = sizeof(T);
	}

	//�C�ӂ̒��_�f�[�^��vector�ōX�V
	template<class T>
	void Update(std::vector<T> list) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (data == nullptr || data->buff.GetRegionPtr() == nullptr) {
			Init(list);
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * list.size());

		if (data->dataSize != dataSize || data->strideInBytes != sizeof(T)) {
			Init(list);
			return;
		}

		T* vertMap = reinterpret_cast<T*>(data->buff.Get());
		for (uint32_t i = 0; i < list.size(); i++) {
			vertMap[i] = list[i];
		}

		data->dataSize = dataSize;
		data->strideInBytes = sizeof(T);
	}

	D3D12_VERTEX_BUFFER_VIEW GetVertView();

private:
	std::shared_ptr<VertexBufferData> data = nullptr;
};

