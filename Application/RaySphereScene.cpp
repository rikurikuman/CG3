#include "RaySphereScene.h"
#include <RImGui.h>
#include <TimeManager.h>
#include <Quaternion.h>

RaySphereScene::RaySphereScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	ray = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));
	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));

	sphere.mTransform.position = { 0, 5, 0 };
	sphere.mTransform.scale = { 0.1f, 0.1f, 0.1f };
	sphere.mTransform.UpdateMatrix();

	sphere2.mTransform.position = { 0, 0, 0 };
	sphere2.mTransform.UpdateMatrix();

	colRay = { {0, 5, 0}, {0, -1, 0} };

	camera.mViewProjection.mEye = { 0, 3, -10 };
	camera.mViewProjection.mTarget = { 0, 3, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void RaySphereScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
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
			sphere2.mTransform.position = { 0, 0, 0 };
			radius = 1;
		}

		ImGui::Text("Ray");
		
		ImGui::Checkbox("AutoMove", &autoMove);
		ImGui::DragFloat3("Position##RayPos", &colRay.start.x, 0.01f);

		ImGui::Text("Sphere");
		ImGui::DragFloat3("Position##SpherePos", &sphere2.mTransform.position.x, 0.01f);
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

	sphere.mTransform.position = colRay.start;
	sphere.mTransform.UpdateMatrix();

	colSphere.pos = sphere2.mTransform.position;
	colSphere.r = radius;
	sphere2.mTransform.scale = { radius, radius, radius };
	sphere2.mTransform.UpdateMatrix();

	float dis = 0;
	Vector3 inter;
	if (ColPrimitive3D::CheckRayToSphere(colRay, colSphere, &dis, &inter)) {
		sphere.mTuneMaterial.mColor = { 1, 0, 0, 1 };
		sphere2.mTuneMaterial.mColor = { 1, 0, 0, 1 };
		ray.mTuneMaterial.mColor = { 0, 0, 1, 1 };
		ray.mTransform.position = colRay.start;
		ray.mTransform.position += colRay.dir * dis / 2.0f;
		ray.mTransform.scale = { 0.1f, dis, 0.1f };
	}
	else {
		sphere.mTuneMaterial.mColor = { 1, 1, 1, 1 };
		sphere2.mTuneMaterial.mColor = { 1, 1, 1, 1 };
		ray.mTuneMaterial.mColor = { 1, 1, 1, 1 };
		ray.mTransform.position = colRay.start;
		ray.mTransform.position += colRay.dir * 50;
		ray.mTransform.scale = { 0.1f, 100, 0.1f };
	}
	ray.mTransform.UpdateMatrix();

	light.Update();
	sphere.TransferBuffer(Camera::sNowCamera->mViewProjection);
	ray.TransferBuffer(Camera::sNowCamera->mViewProjection);
	sphere2.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void RaySphereScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().mPtr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().mPtr.Get());

	sphere.Draw();
	ray.Draw();
	sphere2.Draw();
}
