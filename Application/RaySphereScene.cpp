#include "RaySphereScene.h"
#include <RImGui.h>
#include <TimeManager.h>
#include <Quaternion.h>

RaySphereScene::RaySphereScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	ray = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));
	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));

	sphere.transform.position = { 0, 5, 0 };
	sphere.transform.scale = { 0.1f, 0.1f, 0.1f };
	sphere.transform.UpdateMatrix();

	sphere2.transform.position = { 0, 0, 0 };
	sphere2.transform.UpdateMatrix();

	colRay = { {0, 5, 0}, {0, -1, 0} };

	camera.viewProjection.eye = { 0, 3, -10 };
	camera.viewProjection.target = { 0, 3, 0 };
	camera.viewProjection.UpdateMatrix();
}

void RaySphereScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void RaySphereScene::Update()
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
			sphere2.transform.position = { 0, 0, 0 };
			radius = 1;
		}

		ImGui::Text("Ray");
		
		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position##RayPos", &colRay.start.x, 0.01f);

		ImGui::Text("Sphere");
		ImGui::DragFloat3("Position##SpherePos", &sphere2.transform.position.x, 0.01f);
		ImGui::DragFloat("Radius", &radius, 0.01f);

		ImGui::End();
	}

	if (autoMove) {
		colRay.start.x += 5.0f * TimeManager::deltaTime * moveDir;
		if (colRay.start.x > 5
			|| colRay.start.x < -5) {
			colRay.start.x = Util::Clamp(colRay.start.x, -5.0f, 5.0f);
			moveDir *= -1;
		}
	}

	sphere.transform.position = colRay.start;
	sphere.transform.UpdateMatrix();

	colSphere.pos = sphere2.transform.position;
	colSphere.r = radius;
	sphere2.transform.scale = { radius, radius, radius };
	sphere2.transform.UpdateMatrix();

	float dis = 0;
	Vector3 inter;
	if (ColPrimitive3D::CheckRayToSphere(colRay, colSphere, &dis, &inter)) {
		sphere.tuneMaterial.color = { 1, 0, 0, 1 };
		sphere2.tuneMaterial.color = { 1, 0, 0, 1 };
		ray.tuneMaterial.color = { 0, 0, 1, 1 };
		ray.transform.position = colRay.start;
		ray.transform.position += colRay.dir * dis / 2.0f;
		ray.transform.scale = { 0.1f, dis, 0.1f };
	}
	else {
		sphere.tuneMaterial.color = { 1, 1, 1, 1 };
		sphere2.tuneMaterial.color = { 1, 1, 1, 1 };
		ray.tuneMaterial.color = { 1, 1, 1, 1 };
		ray.transform.position = colRay.start;
		ray.transform.position += colRay.dir * 50;
		ray.transform.scale = { 0.1f, 100, 0.1f };
	}
	ray.transform.UpdateMatrix();

	light.Update();
	sphere.TransferBuffer(Camera::nowCamera->viewProjection);
	ray.TransferBuffer(Camera::nowCamera->viewProjection);
	sphere2.TransferBuffer(Camera::nowCamera->viewProjection);
}

void RaySphereScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());

	sphere.Draw();
	ray.Draw();
	sphere2.Draw();
}
