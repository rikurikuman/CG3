#pragma once
#include "IScene.h"
#include "ColPrimitive3D.h"

class SpherePlaneScene : public IScene
{
public:
	SpherePlaneScene();

private:
	ColPrimitive3D::Sphere colSphere;
	ColPrimitive3D::Plane colPlane;
};

