#include "IRenderStage.h"
#include <RDirectX.h>
#include <RenderTarget.h>

void IRenderStage::AllCall()
{
	RenderOrder memo;

	for (RenderOrder& order : orders) {
		//Ž©“®Ý’è€–Ú‚ÌÝ’è
		if (order.primitiveTopology == D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED) {
			order.primitiveTopology = defParamater.primitiveTopology;
		}
		if (order.renderTargets.empty()) {
			order.renderTargets = defParamater.renderTargets;
		}
		if (order.viewports.empty()) {
			order.viewports = defParamater.viewports;
		}
		if (order.scissorRects.empty()) {
			order.scissorRects = defParamater.scissorRects;
		}
		if (order.rootSignature == nullptr) {
			order.rootSignature = defParamater.rootSignature;
		}
		if (order.pipelineState == nullptr) {
			order.pipelineState = defParamater.pipelineState;
		}

		if (memo.primitiveTopology != order.primitiveTopology) {
			memo.primitiveTopology = order.primitiveTopology;
			RDirectX::GetCommandList()->IASetPrimitiveTopology(order.primitiveTopology);
		}

		if (memo.renderTargets != order.renderTargets) {
			memo.renderTargets = order.renderTargets;
			if (order.renderTargets.empty()) {
				RenderTarget::SetToBackBuffer();
			}
			else {
				RenderTarget::SetToTexture(order.renderTargets);
			}
		}

		if (memo.viewports != order.viewports) {
			memo.viewports = order.viewports;
			std::vector<D3D12_VIEWPORT> vec;
			for (Viewport& vp : order.viewports) {
				D3D12_VIEWPORT viewport{};
				viewport.Width = vp.width;
				viewport.Height = vp.height;
				viewport.TopLeftX = vp.topleftX;
				viewport.TopLeftY = vp.topleftY;
				viewport.MinDepth = vp.minDepth;
				viewport.MaxDepth = vp.maxDepth;
				vec.push_back(viewport);
			}
			RDirectX::GetCommandList()->RSSetViewports(static_cast<UINT>(order.viewports.size()), &vec[0]);
		}

		if (memo.scissorRects != order.scissorRects) {
			memo.scissorRects = order.scissorRects;
			std::vector<D3D12_RECT> vec;
			for (Rect& r : order.scissorRects) {
				D3D12_RECT rect{};
				rect.left = r.left;
				rect.right = r.right;
				rect.top = r.top;
				rect.bottom = r.bottom;
				vec.push_back(rect);
			}
			RDirectX::GetCommandList()->RSSetScissorRects(static_cast<UINT>(order.scissorRects.size()), &vec[0]);
		}

		if (memo.rootSignature != order.rootSignature) {
			memo.rootSignature = order.rootSignature;
			RDirectX::GetCommandList()->SetGraphicsRootSignature(order.rootSignature);
		}

		if (memo.pipelineState != order.pipelineState) {
			memo.pipelineState = order.pipelineState;
			RDirectX::GetCommandList()->SetPipelineState(order.pipelineState);
		}

		RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, order.vertView);

		if (order.indexView != nullptr) {
			RDirectX::GetCommandList()->IASetIndexBuffer(order.indexView);
		}

		int rootIndex = 0;
		for (RootData& data : order.rootData) {
			if (data.type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
				RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(rootIndex, data.descriptor);
			}
			else if (data.type == D3D12_ROOT_PARAMETER_TYPE_CBV) {
				RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(rootIndex, data.address);
			}
			rootIndex++;
		}

		if (order.indexView != nullptr) {
			RDirectX::GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(order.indexCount), 1, 0, 0, 0);
		}
		else {
			RDirectX::GetCommandList()->DrawInstanced(static_cast<UINT>(order.indexCount), 1, 0, 0);
		}
	}
}
