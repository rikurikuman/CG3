#pragma once
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include "RDirectX.h"
#include "Util.h"
#include <chrono>
#include <mutex>
#include <SRBuffer.h>

template <typename T>
class SRConstBuffer
{
private:
	static std::recursive_mutex mutex;
	static std::map<const void*, size_t> countMap;

	static void AddCount(const void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		countMap[p]++;

#ifdef _SRCONSTBUFFER_DEBUG_
		OutputDebugString((std::wstring(L"SRConstBuffer(") + Util::ConvertStringToWString(Util::StringFormat("%p", p)) + L") Add      : " + std::to_wstring(countMap[p]) + L"\n").c_str());
#endif
	}

	static void SubtractCount(const void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		countMap[p]--;

		if (countMap[p] == 0) {
			countMap.erase(p);
		}
	}

	static size_t GetCount(const void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		auto itr = countMap.find(p);
		if (itr == countMap.end()) {
			return 0;
		}
		return countMap[p];
	}

	void Init() {
		buff = SRBufferAllocator::Alloc(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		*reinterpret_cast<T*>(buff.Get()) = T();
	}

public:
	SRBufferPtr buff;

	T* Get() {
		return reinterpret_cast<T*>(buff.Get());
	}

	T* operator->() {
		return reinterpret_cast<T*>(buff.Get());
	}

	SRConstBuffer() {
		Init();
		SRConstBuffer::AddCount(buff.GetRegionPtr());
	}
	SRConstBuffer(const SRConstBuffer& o) {
		if (buff != nullptr) {
			SRConstBuffer::SubtractCount(buff.GetRegionPtr());
			if (SRConstBuffer::GetCount(buff.GetRegionPtr()) == 0) {
				SRBufferAllocator::Free(buff);
			}
		}
		buff = o.buff;
		SRConstBuffer::AddCount(buff.GetRegionPtr());
	}
	~SRConstBuffer() {
		if (buff != nullptr) {
			SRConstBuffer::SubtractCount(buff.GetRegionPtr());
			if (SRConstBuffer::GetCount(buff.GetRegionPtr()) == 0) {
				SRBufferAllocator::Free(buff);
			}
		}
	}
	
	SRConstBuffer& operator=(const SRConstBuffer& o) {
		if (this != &o) {
			if (buff != nullptr) {
				SRConstBuffer::SubtractCount(buff.GetRegionPtr());
				if (SRConstBuffer::GetCount(buff.GetRegionPtr()) == 0) {
					SRBufferAllocator::Free(buff);
				}
			}
			buff = o.buff;
			SRConstBuffer::AddCount(buff.GetRegionPtr());
		}
		return *this;
	}
};

template<typename T>
std::map<const void*, size_t> SRConstBuffer<T>::countMap = std::map<const void*, size_t>();

template<typename T>
std::recursive_mutex SRConstBuffer<T>::mutex;