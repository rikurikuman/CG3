#pragma once
#include "Scene.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "ParticleManager.h"
#include "Sprite.h"
class PolygonScene : public Scene
{
public:
	PolygonScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	bool exploded = false;
	ModelObj obj = ModelObj(Model::Load("Resources/Model/VicViper/", "VicViper.obj"));

	Sprite controlDescText0;
	Sprite controlDescText1;
	Sprite controlDescText2;
	Sprite controlDescText3;

	ParticleManager particleManager;

	DebugCamera camera = DebugCamera({ 0, 0, 0 });
};

