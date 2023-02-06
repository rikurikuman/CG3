#pragma once
#include "IScene.h"
#include "ColPrimitive3D.h"
#include "ModelObj.h"
#include <Camera.h>
#include <LightGroup.h>

class RayPlaneScene : public IScene
{
public:
	RayPlaneScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Camera camera;
	LightGroup light;

	ColPrimitive3D::Ray colRay;
	ColPrimitive3D::Plane colPlane;
	ModelObj sphere;
	ModelObj ray;
	ModelObj plane;

	float rotPlane = 0;

	bool autoMove = true;
	float moveDir = 1;
};

