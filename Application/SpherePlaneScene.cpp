#include "SpherePlaneScene.h"
#include <RImGui.h>
#include <TimeManager.h>
#include <Quaternion.h>

SpherePlaneScene::SpherePlaneScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	plane = ModelObj(Model::Load("./Resources/Model/Ground", "ground.obj", "ground", false));

	sphere.transform.position = { 0, 5, 0 };
	sphere.transform.UpdateMatrix();

	colPlane = { {0, 1, 0}, 0 };

	camera.viewProjection.eye = { 0, 3, -10 };
	camera.viewProjection.target = { 0, 3, 0 };
	camera.viewProjection.UpdateMatrix();
}

void SpherePlaneScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
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
			sphere.transform.position = { 0, 5, 0 };
			sphere.transform.scale = { 1, 1, 1 };
			radius = 1;
			colPlane.distance = 0;
			rotPlane = 0;
		}

		ImGui::Text("Sphere");
		
		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position", &sphere.transform.position.x, 0.01f);
		ImGui::DragFloat("Scale", &radius, 0.01f);

		ImGui::Text("Plane");
		ImGui::DragFloat("Distance", &colPlane.distance, 0.01f);
		ImGui::DragFloat("Rotation", &rotPlane, 0.05f);

		ImGui::End();
	}

	if (autoMove) {
		sphere.transform.position.y += 5.0f * TimeManager::deltaTime * moveDir;
		if (sphere.transform.position.y > 5
			|| sphere.transform.position.y < 0) {
			sphere.transform.position.y = Util::Clamp(sphere.transform.position.y, 0.0f, 5.0f);
			moveDir *= -1;
		}
	}

	colSphere.pos = sphere.transform.position;
	colSphere.r = radius;
	sphere.transform.scale = { radius, radius, radius };
	sphere.transform.UpdateMatrix();

	Vector3 planeN = colPlane.normal.GetNormalize();
	plane.transform.position = planeN * colPlane.distance;
	plane.transform.rotation = Quaternion::AngleAxis({ 0, 0, 1 }, Util::AngleToRadian(rotPlane)).ToEuler();
	plane.transform.UpdateMatrix();

	colPlane.normal = Vector3(0, 1, 0) * Quaternion::AngleAxis({ 0, 0, 1 }, Util::AngleToRadian(rotPlane));

	if (ColPrimitive3D::CheckSphereToPlane(colSphere, colPlane)) {
		sphere.tuneMaterial.color = { 1, 0, 0, 1 };
	}
	else {
		sphere.tuneMaterial.color = { 1, 1, 1, 1 };
	}

	light.Update();
	sphere.TransferBuffer(Camera::nowCamera->viewProjection);
	plane.TransferBuffer(Camera::nowCamera->viewProjection);
}

void SpherePlaneScene::Draw()
{
	sphere.Draw();
	plane.Draw();
}
