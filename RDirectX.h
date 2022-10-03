#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include "Texture.h"
#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Color.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;

class RDirectX
{
public:
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> cmdList = nullptr;
	ComPtr<ID3D12CommandQueue> cmdQueue = nullptr;
	ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> srvHeap = nullptr;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Resource> depthBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;
	Shader basicVS;
	Shader basicPS;
	RootSignature rootSignature;
	GraphicsPipeline pipelineState;

	//DirectX������
	static void Init();
	static RDirectX* GetInstance();

	//���݂̃o�b�N�o�b�t�@�ԍ����擾����
	static UINT GetCurrentBackBufferIndex();

	//���݂̃o�b�N�o�b�t�@�̃��\�[�X���擾����
	static ID3D12Resource* GetCurrentBackBufferResource();

	//���݂̃o�b�N�o�b�t�@��RTV�n���h�����擾����
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle();

	//�����_�[�^�[�Q�b�g���w�肷��
	static void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle);

	//���\�[�X�o���A��PRESENT->RENDER_TARGET�ɂ��鏈��
	static void OpenResorceBarrier(ID3D12Resource* resource);

	//���\�[�X�o���A��RENDER_TARGET->PRESENT�ɂ��鏈��
	static void CloseResourceBarrier(ID3D12Resource* resource);

	//�o�b�N�o�b�t�@��`���ɂ��邽�߂̐F��ȏ������܂Ƃ߂čs��
	static void SetBackBufferToRenderTarget();

	/// <summary>
	/// �����_�[�^�[�Q�b�g���w��œh��Ԃ�
	/// </summary>
	/// <param name="color">�h��Ԃ��F</param>
	static void ClearRenderTarget(Color color);

	//�[�x�l���N���A����
	static void ClearDepthStencil();

	//�R�}���h���X�g����Ď��s���A�t���b�v���āA�R�}���h���X�g���ĂъJ����
	static void RunDraw();

private:
	RDirectX() {};
	~RDirectX() = default;
	RDirectX(const RDirectX& a) {};
	RDirectX& operator=(const RDirectX&) { return *this; }

	D3D12_CPU_DESCRIPTOR_HANDLE nowRtvHandle = {};

	void InitInternal();
};