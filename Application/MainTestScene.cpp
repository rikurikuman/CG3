#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>

MainTestScene::MainTestScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));

	sphere.transform.position = { -2, 0, 0 };
	sphere.transform.UpdateMatrix();

	sphere2 = ModelObj(Model::Load("./Resources/Model/TestSphere", "TestSphere.obj", "TestSphere", true));

	sphere2.transform.position = { 2, 0, 0 };
	sphere2.transform.UpdateMatrix();

	camera.viewProjection.eye = { 0, 0, -10 };
	camera.viewProjection.target = { 0, 0, 0 };
	camera.viewProjection.UpdateMatrix();

	light.SetAmbientColor({ 1, 0, 0 });
}

void MainTestScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;

	toon = RDirectX::GetDefPipeline();
	toon.desc.VS = Shader("Shader/ToonVS.hlsl", "main", "vs_5_0");
	toon.desc.PS = Shader("Shader/ToonPS.hlsl", "main", "ps_5_0");
	toon.Create();
}

void MainTestScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 320 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			useToon = false;
			ambientColor = { 0.3f, 0.3f, 0.3f };
			lightColor = { 1, 1, 1 };
			ambient = { 0.3f, 0, 0 };
			diffuse = { 0.8f, 0, 0 };
			specular = { 1, 1, 1 };
		}

		ImGui::Checkbox("Toon", &useToon);
		ImGui::NewLine();
		ImGui::Text("Light");
		ImGui::ColorEdit3("AmbientColor", &ambientColor.x);
		ImGui::ColorEdit3("LightColor", &lightColor.x);
		ImGui::Checkbox("DirectionalLightActive", &lightActive);
		ImGui::Text("Material");
		ImGui::ColorEdit3("Ambient", &ambient.x);
		ImGui::ColorEdit3("Diffuse", &diffuse.x);
		ImGui::ColorEdit3("Specular", &specular.x);

		ImGui::End();
	}

	light.SetAmbientColor(ambientColor);
	light.SetDirectionalLightColor(0, {lightColor.x, lightColor.y, lightColor.z, 1});
	light.SetDirectionalLightActive(0, lightActive);

	for (std::shared_ptr<ModelData> data : ModelManager::Get("Sphere")->data) {
		data->material.ambient = ambient;
		data->material.diffuse = diffuse;
		data->material.specular = specular;
		data->UpdateMaterial();
	}

	light.Update();
	camera.Update();

	sphere.TransferBuffer(camera.viewProjection);
	sphere2.TransferBuffer(camera.viewProjection);
}

void MainTestScene::Draw()
{
	if (useToon) {
		Renderer::SetPipeline(toon);
	}
	else {
		Renderer::SetPipelineToAuto();
	}
	sphere.Draw();
	sphere2.Draw();
}