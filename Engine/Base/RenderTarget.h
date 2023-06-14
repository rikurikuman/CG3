#pragma once
#include "Texture.h"
#include <vector>

class RenderTargetTexture
{
public:
	std::string name;
	TextureHandle texHandle;
	Color clearColor;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff;
	uint32_t heapIndex = UINT32_MAX;

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle();

	void OpenResourceBarrier();
	void CloseResourceBarrier();
	void ClearRenderTarget();
	void ClearDepthStencil();

private:
	bool isBarrier = true;
};

class RenderTarget
{
public:
	static void SetToBackBuffer();
	static void SetToTexture(std::string name);
	static void SetToTexture(std::vector<std::string> names);

	static void CreateRenderTargetTexture(const uint32_t width, const uint32_t height, const Color clearColor, TextureHandle name);
	static RenderTargetTexture* GetRenderTargetTexture(std::string name);
	static D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t index);
	static D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32_t index);

	static RenderTarget* GetInstance() {
		static RenderTarget instance;
		return &instance;
	}
private:
	RenderTarget() {
		CreateHeaps();
	};
	~RenderTarget() = default;
	RenderTarget(const RenderTarget&) {};
	RenderTarget& operator=(const RenderTarget&) { return *this; }

	void CreateHeaps();

	static const uint32_t numDescriptors = 256;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	std::vector<std::string> currentRenderTargets;
	std::map<std::string, RenderTargetTexture> renderTargetMap;
};

