#include "SpherePlaneScene.h"
#include <RImGui.h>
#include <TimeManager.h>
#include <Quaternion.h>

SpherePlaneScene::SpherePlaneScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	plane = ModelObj(Model::Load("./Resources/Model/Ground", "ground.obj", "ground", false));

	sphere.mTransform.position = { 0, 5, 0 };
	sphere.mTransform.UpdateMatrix();

	colPlane = { {0, 1, 0}, 0 };

	camera.mViewProjection.mEye = { 0, 3, -10 };
	camera.mViewProjection.mTarget = { 0, 3, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void SpherePlaneScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void SpherePlaneScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 250 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			sphere.mTransform.position = { 0, 5, 0 };
			sphere.mTransform.scale = { 1, 1, 1 };
			radius = 1;
			colPlane.distance = 0;
			rotPlane = 0;
		}

		ImGui::Text("Sphere");
		
		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position", &sphere.mTransform.position.x, 0.01f);
		ImGui::DragFloat("Scale", &radius, 0.01f);

		ImGui::Text("Plane");
		ImGui::DragFloat("Distance", &colPlane.distance, 0.01f);
		ImGui::DragFloat("Rotation", &rotPlane, 0.05f);

		ImGui::End();
	}

	if (autoMove) {
		sphere.mTransform.position.y += 5.0f * TimeManager::deltaTime * moveDir;
		if (sphere.mTransform.position.y > 5
			|| sphere.mTransform.position.y < 0) {
			sphere.mTransform.position.y = Util::Clamp(sphere.mTransform.position.y, 0.0f, 5.0f);
			moveDir *= -1;
		}
	}

	colSphere.pos = sphere.mTransform.position;
	colSphere.r = radius;
	sphere.mTransform.scale = { radius, radius, radius };
	sphere.mTransform.UpdateMatrix();

	Vector3 planeN = colPlane.normal.GetNormalize();
	plane.mTransform.position = planeN * colPlane.distance;
	plane.mTransform.rotation = Quaternion::AngleAxis({ 0, 0, 1 }, Util::AngleToRadian(rotPlane)).ToEuler();
	plane.mTransform.UpdateMatrix();

	colPlane.normal = Vector3(0, 1, 0) * Quaternion::AngleAxis({ 0, 0, 1 }, Util::AngleToRadian(rotPlane));

	if (ColPrimitive3D::CheckSphereToPlane(colSphere, colPlane)) {
		sphere.mTuneMaterial.mColor = { 1, 0, 0, 1 };
	}
	else {
		sphere.mTuneMaterial.mColor = { 1, 1, 1, 1 };
	}

	light.Update();
	sphere.TransferBuffer(Camera::sNowCamera->mViewProjection);
	plane.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void SpherePlaneScene::Draw()
{
	sphere.Draw();
	plane.Draw();
}
