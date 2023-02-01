#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class ModelScene : public IScene
{
public:
	ModelScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj model;

	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;

	float rot[3] = { 0, 0, 0 };
	float scale[3] = { 1, 1, 1 };
};

