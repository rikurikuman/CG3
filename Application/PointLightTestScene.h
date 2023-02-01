#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class PointLightTestScene : public IScene
{
public:
	PointLightTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	LightGroup light = LightGroup();
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	ModelObj obj;
	ModelObj obj2;
	ModelObj ground;

	float lightPos[3] = {0, 0, 0};
	float lightColor[3] = { 1, 1, 1 };
	float lightAtten[3] = {0.3f, 0.1f, 0.1f};
};

