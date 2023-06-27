#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"
#include <Bloom.h>
#include <CrossFilter.h>
#include <DepthTest.h>

class MainTestScene : public IScene
{
public:
	MainTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	ModelObj sphere;
	ModelObj sphere2;
	Sprite sprite;
	Sprite sprite2;

	Bloom bloom;
	bool useBloom = true;

	CrossFilter crossFilterA;
	CrossFilter crossFilterB;
	CrossFilter crossFilterC;
	bool useCross = false;
	Vector3 crossFilterAngle = { 0, 45, 135 };
};