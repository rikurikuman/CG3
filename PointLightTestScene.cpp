#include "PointLightTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"

PointLightTestScene::PointLightTestScene()
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
	obj.transform.rotation.z += Util::AngleToRadian(1);
	obj2.transform.rotation.z += Util::AngleToRadian(1);

	obj.transform.UpdateMatrix();
	obj2.transform.UpdateMatrix();

	if (RInput::GetKey(DIK_T)) {
		if (RInput::GetKey(DIK_NUMPAD6)) {
			lightAtten.x += 0.1f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD4)) {
			lightAtten.x -= 0.1f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD8)) {
			lightAtten.y += 0.1f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD2)) {
			lightAtten.y -= 0.1f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD9)) {
			lightAtten.z += 0.1f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD1)) {
			lightAtten.z -= 0.1f * TimeManager::deltaTime;
		}
	}
	else {
		if (RInput::GetKey(DIK_NUMPAD6)) {
			lightPos.x += 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD4)) {
			lightPos.x -= 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD8)) {
			lightPos.z += 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD2)) {
			lightPos.z -= 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD9)) {
			lightPos.y += 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD1)) {
			lightPos.y -= 10.0f * TimeManager::deltaTime;
		}
	}
	

	light.SetPointLightPos(0, lightPos);
	light.SetPointLightAtten(0, lightAtten);

	light.Update();
	camera.Update();

	obj.TransferBuffer(Camera::nowCamera->viewProjection);
	obj2.TransferBuffer(Camera::nowCamera->viewProjection);
	ground.TransferBuffer(Camera::nowCamera->viewProjection);
}

void PointLightTestScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());
	obj.DrawCommands();
	obj2.DrawCommands();
	ground.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	controlDesc.DrawCommands();

	SimpleDrawer::DrawString(10, 32, Util::StringFormat("点光源の座標: {%f, %f, %f}", lightPos.x, lightPos.y, lightPos.z), {1, 1, 1, 1}, "", 20);
	SimpleDrawer::DrawString(20, 52, "テンキーの4,6でX移動, 1,9でY移動, 2,8でZ移動", {1, 1, 1, 1}, "", 15);
	SimpleDrawer::DrawString(10, 70, Util::StringFormat("点光源の減衰係数: {%f, %f, %f}", lightAtten.x, lightAtten.y, lightAtten.z), { 1, 1, 1, 1 }, "", 20);
	SimpleDrawer::DrawString(20, 90, "Tキーを押しながら座標移動操作で減衰係数変更", {1, 1, 1, 1}, "", 15);
}