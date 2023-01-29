#pragma once
#include "ModelObj.h"
#include "ColPrimitive3D.h"
#include "Vector3.h"

class Course {
public:

	struct CheckPoint {
		std::string name;
		ColPrimitive3D::AABB col;
		std::string next = "";
		bool line = false;
	};

	ModelObj courseTerrain;
	std::vector<ColPrimitive3D::Triangle> courseColMeshes;

	std::vector<CheckPoint> checkPoints;

	Course();

	void Update();
	void Draw();
};