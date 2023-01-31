#pragma once
#include "Scene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class SpotLightTestScene : public Scene
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

	Sprite controlDesc;

	Sprite controlDesc1;
	Sprite controlDesc2;
	Sprite controlDesc3;
	Sprite controlDesc4;

	Vector3 lightPos = { 0, 5, 0 };
	Vector3 lightVec = { 0, -1, 0 };
	Vector3 lightAtten = { 0.3f, 0.1f, 0.1f };
	Vector2 lightFactorAngle = { 20.0f, 30.0f };
};

