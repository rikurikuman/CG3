#include "MultiLightTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "RImGui.h"

MultiLightTestScene::MultiLightTestScene()
{
	obj = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "NonSmoothVic", false));
	obj.mTransform.position = { -1, 0, 0 };
	obj.mTransform.scale = { 0.2f, 0.2f, 0.2f };
	obj.mTransform.UpdateMatrix();

	obj2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "SmoothVic", true));
	obj2.mTransform.position = { 1, 0, 0 };
	obj2.mTransform.scale = { 0.2f, 0.2f, 0.2f };
	obj2.mTransform.UpdateMatrix();

	ground = ModelObj(Model::Load("./Resources/Model/Ground", "ground.obj", "ground", true));
	ground.mTransform.position = { 0, -1, 0 };
	ground.mTransform.scale = { 10, 1, 10 };
	ground.mTransform.UpdateMatrix();

	light.SetAmbientColor({ 0.1f, 0.1f, 0.1f });
	light.SetDirectionalLightActive(0, true);
	light.SetDirectionalLightVec(0, { 1, 0, 0 });
	light.SetDirectionalLightColor(0, { 1, 0, 0, 1 });
	light.SetDirectionalLightActive(1, true);
	light.SetDirectionalLightVec(1, { 0, -1, 0 });
	light.SetDirectionalLightColor(1, { 0, 1, 0, 1 });
	light.SetDirectionalLightActive(2, true);
	light.SetDirectionalLightVec(2, { 0, 0, 1 });
	light.SetDirectionalLightColor(2, { 0, 0, 1, 1 });
}

void MultiLightTestScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void MultiLightTestScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 400 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Light Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			light.SetDirectionalLightActive(0, true);
			light.SetDirectionalLightVec(0, { 1, 0, 0 });
			light.SetDirectionalLightColor(0, { 1, 0, 0, 1 });
			light.SetDirectionalLightActive(1, true);
			light.SetDirectionalLightVec(1, { 0, -1, 0 });
			light.SetDirectionalLightColor(1, { 0, 1, 0, 1 });
			light.SetDirectionalLightActive(2, true);
			light.SetDirectionalLightVec(2, { 0, 0, 1 });
			light.SetDirectionalLightColor(2, { 0, 0, 1, 1 });

			activeDir1 = activeDir2 = activeDir3 = true;
			colorDir1[0] = colorDir2[1] = colorDir3[2] = 1;
			colorDir1[1] = colorDir1[2]
				= colorDir2[0] = colorDir2[2]
				= colorDir3[0] = colorDir3[1] = 0;
			vecDir1[0] = vecDir3[2] = 1;
			vecDir2[1] = -1;
			vecDir1[1] = vecDir1[2] 
				= vecDir2[0] = vecDir2[2]
				= vecDir3[0] = vecDir3[1] = 0;
		}

		ImGui::Text("Directional Light 1");
		if (ImGui::Checkbox("Active##Dir1", &activeDir1)) {
			light.SetDirectionalLightActive(0, activeDir1);
		}
		if (ImGui::DragFloat3("Direction##Dir1", vecDir1, 0.01f)) {
			light.SetDirectionalLightVec(0, { vecDir1[0], vecDir1[1], vecDir1[2] });
		}
		if (ImGui::ColorEdit3("Color##Dir1", colorDir1)) {
			light.SetDirectionalLightColor(0, { colorDir1[0], colorDir1[1], colorDir1[2], 1 });
		}

		ImGui::Text("Directional Light 2");
		if (ImGui::Checkbox("Active##Dir2", &activeDir2)) {
			light.SetDirectionalLightActive(1, activeDir2);
		}
		if (ImGui::DragFloat3("Direction##Dir2", vecDir2, 0.01f)) {
			light.SetDirectionalLightVec(1, { vecDir2[0], vecDir2[1], vecDir2[2] });
		}
		if (ImGui::ColorEdit3("Color##Dir2", colorDir2)) {
			light.SetDirectionalLightColor(1, { colorDir2[0], colorDir2[1], colorDir2[2], 1 });
		}

		ImGui::Text("Directional Light 3");
		if (ImGui::Checkbox("Active##Dir3", &activeDir3)) {
			light.SetDirectionalLightActive(2, activeDir3);
		}
		if (ImGui::DragFloat3("Direction##Dir3", vecDir3, 0.01f)) {
			light.SetDirectionalLightVec(2, { vecDir3[0], vecDir3[1], vecDir3[2] });
		}
		if (ImGui::ColorEdit3("Color##Dir3", colorDir3)) {
			light.SetDirectionalLightColor(2, { colorDir3[0], colorDir3[1], colorDir3[2], 1 });
		}

		ImGui::End();
	}

	obj.mTransform.rotation.z += Util::AngleToRadian(1);
	obj2.mTransform.rotation.z += Util::AngleToRadian(1);

	obj.mTransform.UpdateMatrix();
	obj2.mTransform.UpdateMatrix();

	light.Update();
	camera.Update();

	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
	obj2.TransferBuffer(Camera::sNowCamera->mViewProjection);
	ground.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void MultiLightTestScene::Draw()
{
	obj.Draw();
	obj2.Draw();
	ground.Draw();
}