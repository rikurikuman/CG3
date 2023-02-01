#pragma once
#include "Vector3.h"

namespace ColPrimitive3D
{
	//球
	struct Sphere {
		Vector3 pos;
		float r;
	};

	//無限平面
	struct Plane {
		Vector3 normal;
		float distance; //原点からの距離
	};

	//ポリゴン
	struct Triangle {
		Vector3 p0;
		Vector3 p1;
		Vector3 p2;
		Vector3 normal;
	};

	//軸平行ボックス
	struct AABB {
		Vector3 pos;
		Vector3 size;
	};

	//線分
	struct Segment {
		Vector3 start;
		Vector3 end;
		Vector3 dir;
	};

	bool IsHit(Sphere sphere, Plane plane);
	bool IsHit(Sphere sphere, AABB box);
	bool IsHit(Sphere sphere, Triangle triangle, Vector3* outClosestPos = nullptr);
};

