#include "PointLightTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "RImGui.h"

PointLightTestScene::PointLightTestScene()
{
	obj = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "NonSmoothVic", false));
	obj.transform.position = { -1, 0, 0 };
	obj.transform.scale = { 0.2f, 0.2f, 0.2f };
	obj.transform.UpdateMatrix();

	obj2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "SmoothVic", true));
	obj2.transform.position = { 1, 0, 0 };
	obj2.transform.scale = { 0.2f, 0.2f, 0.2f };
	obj2.transform.UpdateMatrix();

	ground = ModelObj(Model::Load("./Resources/Model/Ground", "ground.obj", "ground", true));
	ground.transform.position = { 0, -1, 0 };
	ground.transform.scale = { 10, 1, 10 };
	ground.transform.UpdateMatrix();

	light.SetAmbientColor({ 0.1f, 0.1f, 0.1f });
	light.SetDirectionalLightActive(0, false);

	light.SetPointLightActive(0, true);
	light.SetPointLightPos(0, { 0, 0, 0 });
	light.SetPointLightColor(0, { 1, 1, 1, 1 });
	light.SetPointLightAtten(0, { 0.3f, 0.1f, 0.1f });
}

void PointLightTestScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void PointLightTestScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 200 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("PointLight Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			light.SetPointLightPos(0, { 0, 0, 0 });
			light.SetPointLightColor(0, { 1, 1, 1, 1 });
			light.SetPointLightAtten(0, { 0.3f, 0.1f, 0.1f });

			lightPos[0] = lightPos[1] = lightPos[2] = 0;
			lightColor[0] = lightColor[1] = lightColor[2] = 1;
			lightAtten[0] = 0.3f;
			lightAtten[1] = lightAtten[2] = 0.3f;
		}

		if (ImGui::DragFloat3("Position", lightPos, 0.01f)) {
			light.SetPointLightPos(0, { lightPos[0], lightPos[1], lightPos[2] });
		}
		if (ImGui::ColorEdit3("Color", lightColor)) {
			light.SetPointLightColor(0, { lightColor[0], lightColor[1], lightColor[2], 1 });
		}
		if (ImGui::SliderFloat3("Atten", lightAtten, 0.0f, 1.0f)) {
			light.SetPointLightAtten(0, { lightAtten[0], lightAtten[1], lightAtten[2] });
		}

		ImGui::End();
	}

	obj.transform.rotation.z += Util::AngleToRadian(1);
	obj2.transform.rotation.z += Util::AngleToRadian(1);

	obj.transform.UpdateMatrix();
	obj2.transform.UpdateMatrix();

	light.Update();
	camera.Update();

	obj.TransferBuffer(Camera::nowCamera->viewProjection);
	obj2.TransferBuffer(Camera::nowCamera->viewProjection);
	ground.TransferBuffer(Camera::nowCamera->viewProjection);
}

void PointLightTestScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());
	obj.DrawCommands();
	obj2.DrawCommands();
	ground.DrawCommands();
}