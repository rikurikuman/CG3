#include "SpotLightTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "RImGui.h"

SpotLightTestScene::SpotLightTestScene()
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

	light.SetSpotLightActive(0, true);
	light.SetSpotLightPos(0, { 0, 5, 0 });
	light.SetSpotLightDirection(0, { 0, -1, 0 });
	light.SetSpotLightColor(0, { 1, 1, 1, 1 });
	light.SetSpotLightAtten(0, { 0.0f, 0.0f, 0.0f });
	light.SetSpotLightFactorAngle(0, { Util::AngleToRadian(20), Util::AngleToRadian(30) });
}

void SpotLightTestScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void SpotLightTestScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 200 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("SpotLight Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			light.SetSpotLightPos(0, { 0, 5, 0 });
			light.SetSpotLightDirection(0, { 0, -1, 0 });
			light.SetSpotLightColor(0, { 1, 1, 1, 1 });
			light.SetSpotLightAtten(0, { 0.0f, 0.0f, 0.0f });
			light.SetSpotLightFactorAngle(0, { Util::AngleToRadian(20), Util::AngleToRadian(30) });

			lightPos[0] = lightPos[2] = 0;
			lightPos[1] = 5;
			lightVec[0] = lightVec[2] = 0;
			lightVec[1] = -1;
			lightColor[0] = lightColor[1] = lightColor[2] = 1;
			lightAtten[0] = lightAtten[1] = lightAtten[2] = 0.0f;
			lightFactorAngle[0] = 20.0f;
			lightFactorAngle[1] = 30.0f;
		}

		if (ImGui::DragFloat3("Position", lightPos, 0.01f)) {
			light.SetSpotLightPos(0, { lightPos[0], lightPos[1], lightPos[2] });
		}
		if (ImGui::DragFloat3("Direction", lightVec, 0.01f)) {
			light.SetSpotLightDirection(0, { lightVec[0], lightVec[1], lightVec[2] });
		}
		if (ImGui::ColorEdit3("Color", lightColor)) {
			light.SetSpotLightColor(0, { lightColor[0], lightColor[1], lightColor[2], 1 });
		}
		if (ImGui::SliderFloat3("Atten", lightAtten, 0.0f, 1.0f)) {
			light.SetSpotLightAtten(0, { lightAtten[0], lightAtten[1], lightAtten[2] });
		}
		if (ImGui::DragFloat2("FactorAngle", lightFactorAngle, 0.01f)) {
			light.SetSpotLightFactorAngle(0, { Util::AngleToRadian(lightFactorAngle[0]), Util::AngleToRadian(lightFactorAngle[1]) });
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

void SpotLightTestScene::Draw()
{
	obj.Draw();
	obj2.Draw();
	ground.Draw();
}