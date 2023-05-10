#pragma once
#include "RDirectX.h"
#include "SimpleDrawInfo.h"
#include <mutex>

class SimpleDrawer
{
public:
	static SimpleDrawer* GetInstance() {
		static SimpleDrawer instance;
		return &instance;
	}

	static void ClearData() {
		SimpleDrawer* instance = GetInstance();
		instance->infoList.clear();
		for (auto& itr : instance->recycleBuffs) {
			SRBufferAllocator::Free(*itr.second);
		}
		instance->recycleBuffs.clear();

		instance->circleInstance.clear();
	}

	static void DrawCircle(int x, int y, int r, Color color);
	static void DrawString(float x, float y, std::string text, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f), std::string fontTypeFace = "", UINT fontSize = 20, Vector2 anchor = {0, 0});

	//インスタンシング描画によって提供されるものを全部描画する
	static void DrawInstancing();

private:
	SimpleDrawer() {
		Init();
	};
	~SimpleDrawer() = default;
	SimpleDrawer(const SimpleDrawer& a) = default;
	SimpleDrawer& operator=(const SimpleDrawer&) { return *this; }

	std::list<std::shared_ptr<SimpleDrawInfo>> infoList;
	std::unordered_map<std::string, std::shared_ptr<SRBufferPtr>> recycleBuffs;

	std::unordered_map<std::string, D3D12_VERTEX_BUFFER_VIEW> test;
	SRBufferPtr circleInstanceBuff;
	std::unordered_map<std::string, std::list<std::shared_ptr<DrawCircleInfo>>> circleInstance;

	struct InstancingDrawData {
		RootSignature rootSignature;
		GraphicsPipeline pipelineState;

		SRBufferPtr vert;
		SRBufferPtr index;
		SRBufferPtr inst;
		D3D12_VERTEX_BUFFER_VIEW vertView{};
		D3D12_INDEX_BUFFER_VIEW indexView{};
		D3D12_VERTEX_BUFFER_VIEW instView{};
	};

	RootSignature rootSignature;
	GraphicsPipeline pipelineState;

	RootSignature rootSignatureForCircle;
	GraphicsPipeline pipelineStateForCircle;

	RootSignature rootSignatureForString;
	GraphicsPipeline pipelineStateForString;

	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	std::mutex mutex;

	void Init();
};

