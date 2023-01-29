#pragma once
#include "Scene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class PointLightTestScene : public Scene
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

	Sprite controlDesc;

	Vector3 lightPos = { 0, 0, 0 };
	Vector3 lightAtten = { 0.3f, 0.1f, 0.1f };
};

