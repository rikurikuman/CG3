#include "ModelScene.h"
#include "RImGui.h"

ModelScene::ModelScene()
{
	model = ModelObj(Model::Load("./Resources/Model/VicViper", "VicViper.obj", "Vic", true));
}

void ModelScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void ModelScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 150 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Model Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			rot[0] = 0;
			rot[1] = 0;
			rot[2] = 0;
			scale[0] = 1;
			scale[1] = 1;
			scale[2] = 1;
		}

		ImGui::Text("Transform");
		ImGui::DragFloat3("Rotation", rot);
		ImGui::DragFloat3("Scale", scale, 0.01f);

		ImGui::End();
	}

	model.transform.rotation = { Util::AngleToRadian(rot[0]), Util::AngleToRadian(rot[1]), Util::AngleToRadian(rot[2]) };
	model.transform.scale = { scale[0], scale[1], scale[2]};
	model.transform.UpdateMatrix();

	light.Update();
	camera.Update();

	model.TransferBuffer(camera.viewProjection);
}

void ModelScene::Draw()
{
	model.Draw();
}
