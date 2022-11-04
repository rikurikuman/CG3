#pragma once
#include <memory>
#include "ViewProjection.h"
#include "RWindow.h"

class Camera
{
protected:
	//ViewProjection viewProjection;

public:
	ViewProjection viewProjection;

	Camera() {
		viewProjection.aspect = (float)RWindow::GetWidth() / RWindow::GetHeight();
		viewProjection.UpdateMatrix();
	}
	virtual ~Camera() {}

	virtual void Update() {}

	static std::unique_ptr<Camera> _def;
	static Camera* nowCamera;
};

