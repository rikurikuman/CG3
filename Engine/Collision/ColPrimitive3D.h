#pragma once
#include "Vector3.h"

namespace ColPrimitive3D
{
	//��
	struct Sphere {
		Vector3 pos;
		float r;
	};

	//��������
	struct Plane {
		Vector3 normal;
		float distance; //���_����̋���
	};

	//�|���S��
	struct Triangle {
		Vector3 p0;
		Vector3 p1;
		Vector3 p2;
		Vector3 normal;
	};

	//�����s�{�b�N�X
	struct AABB {
		Vector3 pos;
		Vector3 size;
	};

	//����
	struct Segment {
		Vector3 start;
		Vector3 end;
		Vector3 dir;
	};

	bool IsHit(Sphere sphere, Plane plane);
	bool IsHit(Sphere sphere, AABB box);
	bool IsHit(Sphere sphere, Triangle triangle, Vector3* outClosestPos = nullptr);
};

