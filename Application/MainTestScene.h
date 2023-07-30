#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"
#include <MultiRenderTest.h>
#include <DissolveModel.h>
#include <PerlinNoise.h>

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

	ModelObj skydome;

	PerlinNoise noise;
	DissolveModel obj;
};