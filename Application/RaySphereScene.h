#pragma once
#include "IScene.h"
#include "ColPrimitive3D.h"
#include "ModelObj.h"
#include <Camera.h>
#include <LightGroup.h>

class RaySphereScene : public IScene
{
public:
	RaySphereScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Camera camera;
	LightGroup light;

	ColPrimitive3D::Ray colRay;
	ColPrimitive3D::Sphere colSphere;
	ModelObj sphere;
	ModelObj ray;
	ModelObj sphere2;

	float radius = 1;

	bool autoMove = true;
	float moveDir = 1;
};

