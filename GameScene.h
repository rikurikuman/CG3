#pragma once
#include "Scene.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "Cube.h"
#include "DebugCamera.h"

class GameScene : public Scene
{
public:
	GameScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;

	Sprite controlDescText1;
	Sprite controlDescText2;
	Sprite controlDescText3;

	Cube cube;
	ModelObj model;

	Image3D text;

	DebugCamera camera = DebugCamera({0, 0, 0});

	//糞雑グリッド用
	RootSignature gridRoot;
	GraphicsPipeline gridPSO;
	std::vector<VertexP> gridVertices;
	VertexBuffer gridVertBuff;
	Material gridMaterial;
	RConstBuffer<MaterialBuffer> gridMaterialBuff;
	RConstBuffer<ViewProjectionBuffer> gridViewProjectionBuff;
};

