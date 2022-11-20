#pragma once
#include "Scene.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "Cube.h"
#include "DebugCamera.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;

	Sprite controlDescText0;
	Sprite controlDescText1;
	Sprite controlDescText2;
	Sprite controlDescText3;

	Cube cube;

	Image3D text;

	ParticleManager particleManager;
	ParticleEmitter* patEmitterA = nullptr;
	ParticleEmitter* patEmitterB = nullptr;
	ParticleEmitter* patEmitterC = nullptr;

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

