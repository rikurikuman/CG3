#pragma once
#include "Scene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class MultiLightTestScene : public Scene
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

	Sprite controlDesc;
};

