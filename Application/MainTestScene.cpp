#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"

MainTestScene::MainTestScene()
{
	controlDesc = Sprite(TextDrawer::CreateStringTexture("マウス:視線, WASD:移動, Space:上昇, LShift:下降", "", 32), {0, 1});
	controlDesc.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDesc.transform.UpdateMatrix();
	controlDesc.material.color = { 1, 1, 1, 1 };
	controlDesc.TransferBuffer();

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
	light.SetDirectionalLightActive(0, true);
}

void MainTestScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void MainTestScene::Update()
{
	obj.transform.rotation.z += Util::AngleToRadian(1);
	obj2.transform.rotation.z += Util::AngleToRadian(1);

	obj.transform.UpdateMatrix();
	obj2.transform.UpdateMatrix();

	if (RInput::GetKey(DIK_NUMPAD6)) {
		lightVec.x += 1.0f * TimeManager::deltaTime;
	}
	if (RInput::GetKey(DIK_NUMPAD4)) {
		lightVec.x -= 1.0f * TimeManager::deltaTime;
	}
	if (RInput::GetKey(DIK_NUMPAD8)) {
		lightVec.z += 1.0f * TimeManager::deltaTime;
	}
	if (RInput::GetKey(DIK_NUMPAD2)) {
		lightVec.z -= 1.0f * TimeManager::deltaTime;
	}
	if (RInput::GetKey(DIK_NUMPAD9)) {
		lightVec.y += 1.0f * TimeManager::deltaTime;
	}
	if (RInput::GetKey(DIK_NUMPAD1)) {
		lightVec.y -= 1.0f * TimeManager::deltaTime;
	}

	light.SetDirectionalLightVec(0, lightVec);

	light.Update();
	camera.Update();

	obj.TransferBuffer(Camera::nowCamera->viewProjection);
	obj2.TransferBuffer(Camera::nowCamera->viewProjection);
	ground.TransferBuffer(Camera::nowCamera->viewProjection);
}

void MainTestScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());
	obj.DrawCommands();
	obj2.DrawCommands();
	ground.DrawCommands();

	RDirectX::GetCommandList()->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());
	RDirectX::GetCommandList()->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	controlDesc.DrawCommands();

	SimpleDrawer::DrawString(10, 32, Util::StringFormat("ライトの向き: {%f, %f, %f}", lightVec.x, lightVec.y, lightVec.z), {1, 1, 1, 1}, "");
	SimpleDrawer::DrawString(20, 52, "テンキーの4,6でX移動, 1,9でY移動, 2,8でZ移動", {1, 1, 1, 1}, "", 15);
}