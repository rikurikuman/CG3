#pragma once
#include "Scene.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "Cube.h"
#include "DebugCamera.h"
#include "Light.h"
#include "Player.h"
#include "ColPrimitive3D.h"
#include "Course.h"

class GameScene : public Scene
{
public:
	GameScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Sprite controlDescText1;
	Sprite controlDescText2;
	Sprite controlDescText3;

	Sprite countText1;
	Sprite countText2;
	Sprite countText3;
	Sprite countText4;
	Sprite goalText;

	Sprite descImg;
	Sprite monitorImg;
	Sprite meterImg;

	Player player;
	Course course;

	float runTimer = 0;

	bool startFlag = false;
	float startTimer = 0;
	int startCount = 0;

	bool goalFlag = false;
	float goalTimer = 0;

	DebugCamera camera = DebugCamera({0, 0, 0});
	Light light = Light();
};

