#pragma once
#include "Vector3.h"
#include "RConstBuffer.h"

class Light
{
public:
	struct LightBuffer
	{
		Vector3 lightVec;
		float pad;
		Vector3 color;
	};

	RConstBuffer<LightBuffer> buffer;

	Vector3 lightVec;
	Color color;

	Light() : lightVec(Vector3(0, -1, 0)), color(Color(1, 1, 1, 1)) {
		TransferBuffer();
	}
	Light(Vector3 lightVec, Color color) : lightVec(lightVec), color(color) {
		TransferBuffer();
	}

	void TransferBuffer() {
		buffer.constMap->lightVec = lightVec;
		buffer.constMap->color = Vector3(color.r, color.g, color.b);
	}

	static Light* nowLight;
};

