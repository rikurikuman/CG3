#pragma once
#include "Scene.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Light.h"
#include "Sprite.h"

class TitleScene : public Scene
{
public:
	TitleScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Light light = Light();
	Camera camera;
	ModelObj objVic;

	Sprite logo;
	Sprite startBar;
	float startTimer = 0;
};

