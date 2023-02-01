#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class MultiLightTestScene : public IScene
{
public:
	MultiLightTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	LightGroup light = LightGroup();
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	ModelObj obj;
	ModelObj obj2;

	ModelObj ground;

	bool activeDir1 = true;
	bool activeDir2 = true;
	bool activeDir3 = true;
	float colorDir1[3] = { 1, 0, 0 };
	float colorDir2[3] = { 0, 1, 0 };
	float colorDir3[3] = { 0, 0, 1 };
	float vecDir1[3] = { 1, 0, 0 };
	float vecDir2[3] = { 0, -1, 0 };
	float vecDir3[3] = { 0, 0, 1 };
};

