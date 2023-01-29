#pragma once
#include "ModelObj.h"
#include "Camera.h"
#include "Course.h"

class Player
{
public:
	ModelObj obj;
	Transform transform;
	ColPrimitive3D::Sphere col;

	Vector3 velocity;
	float speed = 0;
	float brake = 0;
	float charge = 0;
	bool charging = false;

	Course* course = nullptr;
	std::string prevCheckPoints = "";
	std::string nextCheckPoints = "";

	bool initFlag = false;
	bool startFlag = false;
	int lap = 0;

	Camera camera;

	Player();

	void Update();
	void Draw();
};

