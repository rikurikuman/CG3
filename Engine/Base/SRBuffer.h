#pragma once
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <list>
#include <mutex>

struct MemoryRegion {
	byte* pBegin = nullptr;
	byte* pEnd = nullptr;
	size_t size = 0;
	uint32_t align = 0;
	std::list<MemoryRegion>::iterator memItr;

	MemoryRegion() {}
	MemoryRegion(byte* pBegin, byte* pEnd, uint32_t align = 0)
		: pBegin(pBegin), pEnd(pEnd), size(size_t(pEnd - pBegin + 1)), align(align) {}
};

struct MemoryRegionPtr {
	MemoryRegion* region = nullptr;
	std::list<MemoryRegionPtr>::iterator memItr;
};

class SRBufferPtr {
public:
	SRBufferPtr() {}

	byte* Get();

	const MemoryRegionPtr* GetRegionPtr() const {
		return ptr_;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	bool operator==(const void* ptr) const {
		return ptr_ == ptr;
	}
	bool operator!=(const void* ptr) const {
		return ptr_ != ptr;
	}
	bool operator==(const SRBufferPtr& ptr) const {
		return ptr_ == ptr.ptr_;
	}
	bool operator!=(const SRBufferPtr& ptr) const {
		return !(*this == ptr);
	}

	operator bool() const;

private:
	friend class SRBufferAllocator;
	SRBufferPtr(MemoryRegionPtr* ptr) : ptr_(ptr) {}
	MemoryRegionPtr* ptr_ = nullptr;
};

class SRBufferAllocator
{
public:
	static std::recursive_mutex mutex; //ñ{ìñÇÕåˆäJÇµÇΩÇ≠Ç»Ç¢ÇÒÇæÇØÇ«ç°ÇÕë√ã¶
	static bool optAutoDeflag;
	static bool optAutoReCreateBuffer;

	static SRBufferAllocator* GetInstance();

	static SRBufferPtr Alloc(size_t needSize, uint32_t align);
	static void Free(SRBufferPtr& ptr);
	
	static D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		return GetInstance()->buffer->GetGPUVirtualAddress();
	}

	static byte* GetBufferAddress() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		return GetInstance()->pBufferBegin;
	}

	static size_t GetUsingBufferSize() {
		return GetInstance()->usingBufferSizeCounter;
	}

	static size_t GetStrictUsingBufferSize() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		size_t size = 0;
		for (auto& itr : GetInstance()->usingRegions) {
			size += static_cast<size_t>(itr.size);
		}
		return size;
	}

	static size_t GetBufferSize() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
		return static_cast<size_t>(GetInstance()->pBufferEnd - GetInstance()->pBufferBegin + 1);
	}

	void DeFlag();
	void ResizeBuffer();

private:
	constexpr static size_t defSize = 1024 * 1024 * 64; //64MB

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer = nullptr;
	UINT8* pBufferBegin = nullptr;
	UINT8* pBufferEnd = nullptr;
	std::list<MemoryRegion> freeRegions;
	std::list<MemoryRegion> usingRegions;
	std::list<MemoryRegionPtr> regionPtrs;
	size_t usingBufferSizeCounter = 0;

	static MemoryRegion* _Alloc(size_t needSize, uint32_t align, bool deflag);
	//static void _Free(byte* ptr);
	static void _Free(SRBufferPtr& ptr);

	SRBufferAllocator();
	SRBufferAllocator(const SRBufferAllocator& a) = delete;
	SRBufferAllocator& operator=(const SRBufferAllocator&) = delete;
};

