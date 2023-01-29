#pragma once
#include "Scene.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Light.h"
#include "Sprite.h"

class ResultScene : public Scene
{
public:
	ResultScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Light light = Light();
	Camera camera;
	ModelObj objC;

	Sprite screen;
};

