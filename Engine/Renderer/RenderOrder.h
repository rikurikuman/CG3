#pragma once
#include <d3d12.h>
#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include <Viewport.h>
#include <Rect.h>
#include <Vector3.h>

struct RootData {
	D3D12_ROOT_PARAMETER_TYPE type;
	union {
		D3D12_GPU_DESCRIPTOR_HANDLE descriptor;
		D3D12_GPU_VIRTUAL_ADDRESS address;
	};

	RootData() 
		: type(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE), address(0) {}
	RootData(D3D12_GPU_DESCRIPTOR_HANDLE& descriptor)
		: type(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE), descriptor(descriptor) {}
	RootData(D3D12_ROOT_PARAMETER_TYPE type, D3D12_GPU_VIRTUAL_ADDRESS address)
		: type(type), address(address) {}
};

class RenderOrder
{
public:
	Vector3 anchorPoint;
	std::vector<std::string> renderTargets;
	std::vector<Viewport> viewports;
	std::vector<Rect> scissorRects;
	D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* pipelineState = nullptr;
	D3D12_VERTEX_BUFFER_VIEW* vertView = nullptr;
	D3D12_INDEX_BUFFER_VIEW* indexView = nullptr;
	UINT indexCount = 0;
	std::vector<RootData> rootData;

	void Execute();
};

