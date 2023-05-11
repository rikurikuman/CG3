#pragma once
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <list>
#include <mutex>

struct MemoryRegion {
	byte* pBegin = nullptr;
	byte* pEnd = nullptr;
	UINT64 size = 0;
	UINT align = 0;

	MemoryRegion() {}
	MemoryRegion(byte* pBegin, byte* pEnd, UINT align = 0)
		: pBegin(pBegin), pEnd(pEnd), size(UINT64(pEnd - pBegin + 1)), align(align) {}
};

struct MemoryRegionPtr {
	MemoryRegion* region = nullptr;
};

class SRBufferPtr {
public:
	SRBufferPtr() {}

	byte* Get() {
		return ptr->region->pBegin;
	}

	const MemoryRegionPtr* GetRegionPtr() {
		return ptr;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

	bool operator==(const void* ptr) {
		return this->ptr == ptr;
	}
	bool operator!=(const void* ptr) {
		return !(this == ptr);
	}

	operator bool() {
		return ptr != nullptr && ptr->region != nullptr;
	}

private:
	friend class SRBufferAllocator;
	SRBufferPtr(MemoryRegionPtr* ptr) : ptr(ptr) {}
	MemoryRegionPtr* ptr = nullptr;
};

class SRBufferAllocator
{
public:
	std::recursive_mutex mutex; //ñ{ìñÇÕåˆäJÇµÇΩÇ≠Ç»Ç¢ÇÒÇæÇØÇ«ç°ÇÕë√ã¶
	static bool optAutoDeflag;
	static bool optAutoReCreateBuffer;

	static SRBufferAllocator* GetInstance();

	static SRBufferPtr Alloc(UINT64 needSize, UINT align);
	static void Free(SRBufferPtr& ptr);
	
	static D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		return GetInstance()->buffer->GetGPUVirtualAddress();
	}

	static byte* GetBufferAddress() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		return GetInstance()->pBufferBegin;
	}

	static UINT64 GetUsingBufferSize() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		UINT64 size = 0;
		for (auto& itr : GetInstance()->usingRegions) {
			size += static_cast<UINT64>(itr.size);
		}
		return size;
	}

	static UINT64 GetBufferSize() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		return static_cast<UINT64>(GetInstance()->pBufferEnd - GetInstance()->pBufferBegin + 1);
	}

	void DeFlag();
	void ResizeBuffer();

private:
	constexpr static UINT64 defSize = 1024 * 1024 * 16; //16MB

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer = nullptr;
	UINT8* pBufferBegin = nullptr;
	UINT8* pBufferEnd = nullptr;
	std::list<MemoryRegion> freeRegions;
	std::list<MemoryRegion> usingRegions;
	std::list<MemoryRegionPtr> regionPtrs;

	static MemoryRegion* _Alloc(UINT64 needSize, UINT align, bool deflag);
	static void _Free(byte* ptr);

	SRBufferAllocator();
	SRBufferAllocator(const SRBufferAllocator& a) = delete;
	SRBufferAllocator& operator=(const SRBufferAllocator&) = delete;
};

