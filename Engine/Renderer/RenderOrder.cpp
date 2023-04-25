#include "RenderOrder.h"
#include <RDirectX.h>
#include <RenderTarget.h>

void RenderOrder::Execute()
{
	RDirectX::GetCommandList()->IASetPrimitiveTopology(primitiveTopology);

	if (renderTargets.empty()) {
		RenderTarget::SetToBackBuffer();
	}
	else {
		RenderTarget::SetToTexture(renderTargets);
	}

	std::vector<D3D12_VIEWPORT> _viewports;
	for (Viewport& vp : viewports) {
		D3D12_VIEWPORT viewport{};
		viewport.Width = vp.width;
		viewport.Height = vp.height;
		viewport.TopLeftX = vp.topleftX;
		viewport.TopLeftY = vp.topleftY;
		viewport.MinDepth = vp.minDepth;
		viewport.MaxDepth = vp.maxDepth;
		_viewports.push_back(viewport);
	}
	RDirectX::GetCommandList()->RSSetViewports(static_cast<UINT>(viewports.size()), &_viewports[0]);

	std::vector<D3D12_RECT> _rects;
	for (Rect& r : scissorRects) {
		D3D12_RECT rect{};
		rect.left = r.left;
		rect.right = r.right;
		rect.top = r.top;
		rect.bottom = r.bottom;
		_rects.push_back(rect);
	}
	RDirectX::GetCommandList()->RSSetScissorRects(static_cast<UINT>(scissorRects.size()), &_rects[0]);

	RDirectX::GetCommandList()->SetGraphicsRootSignature(rootSignature);
	RDirectX::GetCommandList()->SetPipelineState(pipelineState);

	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, vertView);
	if (indexView != nullptr) {
		RDirectX::GetCommandList()->IASetIndexBuffer(indexView);
	}

	int rootIndex = 0;
	for (RootData& data : rootData) {
		if (data.type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(rootIndex, data.descriptor);
		}
		else if (data.type == D3D12_ROOT_PARAMETER_TYPE_CBV) {
			RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(rootIndex, data.address);
		}
		rootIndex++;
	}

	if (indexView != nullptr) {
		RDirectX::GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(indexCount), 1, 0, 0, 0);
	}
	else {
		RDirectX::GetCommandList()->DrawInstanced(static_cast<UINT>(indexCount), 1, 0, 0);
	}
}
