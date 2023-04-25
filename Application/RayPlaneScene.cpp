#include "RayPlaneScene.h"
#include <RImGui.h>
#include <TimeManager.h>
#include <Quaternion.h>

RayPlaneScene::RayPlaneScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	ray = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));
	plane = ModelObj(Model::Load("./Resources/Model/Ground", "ground.obj", "ground", false));

	sphere.transform.position = { 0, 5, 0 };
	sphere.transform.scale = { 0.1f, 0.1f, 0.1f };
	sphere.transform.UpdateMatrix();

	colRay = { {0, 5, 0}, {0, -1, 0} };

	colPlane = { {0, 1, 0}, 2 };

	camera.viewProjection.eye = { 0, 3, -10 };
	camera.viewProjection.target = { 0, 3, 0 };
	camera.viewProjection.UpdateMatrix();
}

void RayPlaneScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void RayPlaneScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 250 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			colRay.start = { 0, 5, 0 };
			colPlane.distance = 2;
			rotPlane = 0;
		}

		ImGui::Text("Ray");
		
		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position", &colRay.start.x, 0.01f);

		ImGui::Text("Plane");
		ImGui::DragFloat("Distance", &colPlane.distance, 0.01f);
		ImGui::DragFloat("Rotation", &rotPlane, 0.05f);

		ImGui::End();
	}

	if (autoMove) {
		colRay.start.y += 5.0f * TimeManager::deltaTime * moveDir;
		if (colRay.start.y > 5
			|| colRay.start.y < 0) {
			colRay.start.y = Util::Clamp(colRay.start.y, 0.0f, 5.0f);
			moveDir *= -1;
		}
	}

	sphere.transform.position = colRay.start;
	sphere.transform.UpdateMatrix();

	Vector3 planeN = colPlane.normal.GetNormalize();
	plane.transform.position = Vector3(0, 0, 100) + planeN * colPlane.distance;
	plane.transform.rotation = Quaternion::AngleAxis({ 0, 0, 1 }, Util::AngleToRadian(rotPlane)).ToEuler();
	plane.transform.UpdateMatrix();

	colPlane.normal = Vector3(0, 1, 0) * Quaternion::AngleAxis({ 0, 0, 1 }, Util::AngleToRadian(rotPlane));

	float dis = 0;
	Vector3 inter;
	if (ColPrimitive3D::CheckRayToPlane(colRay, colPlane, &dis, &inter)) {
		sphere.tuneMaterial.color = { 1, 0, 0, 1 };
		ray.tuneMaterial.color = { 0, 0, 1, 1 };
		ray.transform.position = colRay.start;
		ray.transform.position += colRay.dir * dis / 2.0f;
		ray.transform.scale = { 0.1f, dis, 0.1f };
	}
	else {
		sphere.tuneMaterial.color = { 1, 1, 1, 1 };
		ray.tuneMaterial.color = { 1, 1, 1, 1 };
		ray.transform.position = colRay.start;
		ray.transform.position += colRay.dir * 50;
		ray.transform.scale = { 0.1f, 100, 0.1f };
	}
	ray.transform.UpdateMatrix();

	light.Update();
	sphere.TransferBuffer(Camera::nowCamera->viewProjection);
	ray.TransferBuffer(Camera::nowCamera->viewProjection);
	plane.TransferBuffer(Camera::nowCamera->viewProjection);
}

void RayPlaneScene::Draw()
{
	sphere.Draw();
	ray.Draw();
	plane.Draw();
}
