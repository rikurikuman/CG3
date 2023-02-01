#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class SpotLightTestScene : public IScene
{
public:
	SpotLightTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	LightGroup light = LightGroup();
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	ModelObj obj;
	ModelObj obj2;
	ModelObj ground;

	float lightPos[3] = { 0, 5, 0 };
	float lightVec[3] = { 0, -1, 0 };
	float lightColor[3] = { 1, 1, 1 };
	float lightAtten[3] = { 0.0f, 0.0f, 0.0f };
	float lightFactorAngle[2] = {20.0f, 30.0f};
};

