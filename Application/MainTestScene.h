#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "PostEffect.h"
#include "SRConstBuffer.h"

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

	bool useToon = false;
	Vector3 ambientColor = { 0.3f, 0.3f, 0.3f };
	Vector3 lightColor = { 1, 1, 1 };
	bool lightActive = true;
	Vector3 ambient = { 0.3f, 0, 0 };
	Vector3 diffuse = { 0.8f, 0, 0 };
	Vector3 specular = { 1, 1, 1 };

	GraphicsPipeline toon;
};