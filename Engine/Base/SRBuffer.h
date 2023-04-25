#pragma once
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <list>

class SRBufferAllocator
{
public:
	class MemoryRegion {
	public:
		UINT8* pBegin = nullptr;
		UINT8* pEnd = nullptr;
		UINT64 size = 0;

		MemoryRegion() {}
		MemoryRegion(UINT8* pBegin, UINT8* pEnd)
			: pBegin(pBegin), pEnd(pEnd), size(UINT64(pEnd - pBegin)) {}
	};

	static SRBufferAllocator* GetInstance();

	static UINT8* Alloc(UINT64 needSize, UINT align);
	static void Free(UINT8* ptr);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer = nullptr;
	UINT8* pBufferBegin = nullptr;
	UINT8* pBufferEnd = nullptr;
	std::list<MemoryRegion> freeRegions;
	std::list<MemoryRegion> usingRegions;

	SRBufferAllocator();
	~SRBufferAllocator() = default;
	SRBufferAllocator(const SRBufferAllocator& a) = delete;
	SRBufferAllocator& operator=(const SRBufferAllocator&) = delete;
};

