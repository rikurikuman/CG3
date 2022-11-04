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
		GetInstance()->infoList.clear();
	}

	static void DrawBox(float x1, float y1, float x2, float y2, Color color, bool fillFlag);
	static void DrawString(float x, float y, std::string text, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f), std::string fontTypeFace = "", UINT fontSize = 20, Vector2 anchor = {0, 0});

private:
	SimpleDrawer() {
		Init();
	};
	~SimpleDrawer() = default;
	SimpleDrawer(const SimpleDrawer& a) = default;
	SimpleDrawer& operator=(const SimpleDrawer&) { return *this; }

	std::list<std::shared_ptr<SimpleDrawInfo>> infoList;
	RootSignature rootSignature;
	GraphicsPipeline pipelineState;

	RootSignature rootSignatureForString;
	GraphicsPipeline pipelineStateForString;

	std::mutex mutex;

	void Init();
};

