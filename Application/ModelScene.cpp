#include "ModelScene.h"
#include "RImGui.h"

ModelScene::ModelScene()
{
	model = ModelObj(Model::Load("./Resources/Model/VicViper", "VicViper.obj", "Vic", true));
}

void ModelScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
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

	model.mTransform.rotation = { Util::AngleToRadian(rot[0]), Util::AngleToRadian(rot[1]), Util::AngleToRadian(rot[2]) };
	model.mTransform.scale = { scale[0], scale[1], scale[2]};
	model.mTransform.UpdateMatrix();

	light.Update();
	camera.Update();

	model.TransferBuffer(camera.mViewProjection);
}

void ModelScene::Draw()
{
	model.Draw();
}
