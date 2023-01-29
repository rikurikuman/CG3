#pragma once
#include "Scene.h"
#include "Light.h"
#include "DebugCamera.h"
#include "ModelObj.h"

class TestScene : public Scene
{
public:
	TestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Light light = Light();
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	ModelObj obj;
	ModelObj obj2;
};

