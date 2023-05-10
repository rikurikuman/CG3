#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "PostEffect.h"
#include "SRConstBuffer.h"

class MainTestScene : public IScene
{
public:
	MainTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	ModelObj sphere;
	ModelObj sphere2;
	Sprite sprite;
	Sprite sprite2;

	RootSignature testRootSignature;
	GraphicsPipeline testPipeline;

	SRBufferPtr vertBuffT;
	D3D12_VERTEX_BUFFER_VIEW vertBuffView{};
	SRBufferPtr instanceVertBuffT;
	D3D12_VERTEX_BUFFER_VIEW instanceVertBuffView{};
	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuffT;

	PostEffect hoge;

	float radius = 1;
};