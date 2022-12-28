#pragma once
#include "Scene.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "Cube.h"
#include "DebugCamera.h"

class GameScene : public Scene
{
public:
	GameScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;

	Sprite controlDescText1;
	Sprite controlDescText2;
	Sprite controlDescText3;

	Cube floor;
	ModelObj objVicViper;
	float speed = 0;
	float brake = 0;
	float charge = 0;

	DebugCamera camera = DebugCamera({0, 0, 0});
};

