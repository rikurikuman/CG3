#include "SpotLightTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"

SpotLightTestScene::SpotLightTestScene()
{
	controlDesc = Sprite(TextDrawer::CreateStringTexture("マウス:視線, WASD:移動, Space:上昇, LShift:下降", "", 32), {0, 1});
	controlDesc.transform.position = { 0, (float)RWindow::GetHeight(), 0 };
	controlDesc.transform.UpdateMatrix();
	controlDesc.TransferBuffer();

	controlDesc1 = Sprite(TextDrawer::CreateStringTexture("テンキーの4,6でX移動, 1,9でY移動, 2,8でZ移動", "", 15, "control1"), {0, 0});
	controlDesc1.transform.position = { 20, 52, 0 };
	controlDesc1.transform.UpdateMatrix();
	controlDesc1.TransferBuffer();

	controlDesc2 = Sprite(TextDrawer::CreateStringTexture("Vキーを押しながら座標移動操作で向き変更", "", 15, "control2"), { 0, 0 });
	controlDesc2.transform.position = { 20, 90, 0 };
	controlDesc2.transform.UpdateMatrix();
	controlDesc2.TransferBuffer();

	controlDesc3 = Sprite(TextDrawer::CreateStringTexture("Tキーを押しながら座標移動操作で減衰係数変更", "", 15, "control3"), { 0, 0 });
	controlDesc3.transform.position = { 20, 130, 0 };
	controlDesc3.transform.UpdateMatrix();
	controlDesc3.TransferBuffer();

	controlDesc4 = Sprite(TextDrawer::CreateStringTexture("Yキーを押しながら座標移動操作で減衰角度変更", "", 15, "control4"), { 0, 0 });
	controlDesc4.transform.position = { 20, 170, 0 };
	controlDesc4.transform.UpdateMatrix();
	controlDesc4.TransferBuffer();

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
	light.SetSpotLightPos(0, { 0, 0, 0 });
	light.SetSpotLightColor(0, { 1, 1, 1, 1 });
	light.SetSpotLightAtten(0, { 0.3f, 0.1f, 0.1f });
}

void SpotLightTestScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void SpotLightTestScene::Update()
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
	else if (RInput::GetKey(DIK_V)) {
		if (RInput::GetKey(DIK_NUMPAD6)) {
			lightVec.x += 1.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD4)) {
			lightVec.x -= 1.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD8)) {
			lightVec.y += 1.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD2)) {
			lightVec.y -= 1.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD9)) {
			lightVec.z += 1.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD1)) {
			lightVec.z -= 1.0f * TimeManager::deltaTime;
		}
	}
	else if (RInput::GetKey(DIK_Y)) {
		if (RInput::GetKey(DIK_NUMPAD6)) {
			lightFactorAngle.x += 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD4)) {
			lightFactorAngle.x -= 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD8)) {
			lightFactorAngle.y += 10.0f * TimeManager::deltaTime;
		}
		if (RInput::GetKey(DIK_NUMPAD2)) {
			lightFactorAngle.y -= 10.0f * TimeManager::deltaTime;
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
	
	light.SetSpotLightPos(0, lightPos);
	light.SetSpotLightDirection(0, lightVec.GetNormalize());
	light.SetSpotLightAtten(0, lightAtten);
	light.SetSpotLightFactorAngle(0, { Util::AngleToRadian(lightFactorAngle.x), Util::AngleToRadian(lightFactorAngle.y) });

	light.Update();
	camera.Update();

	obj.TransferBuffer(Camera::nowCamera->viewProjection);
	obj2.TransferBuffer(Camera::nowCamera->viewProjection);
	ground.TransferBuffer(Camera::nowCamera->viewProjection);
}

void SpotLightTestScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());
	obj.DrawCommands();
	obj2.DrawCommands();
	ground.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	controlDesc.DrawCommands();
	controlDesc1.DrawCommands();
	controlDesc2.DrawCommands();
	controlDesc3.DrawCommands();
	controlDesc4.DrawCommands();

	SimpleDrawer::DrawString(10, 32, Util::StringFormat("スポットライトの座標: {%f, %f, %f}", lightPos.x, lightPos.y, lightPos.z), {1, 1, 1, 1}, "", 20);
	SimpleDrawer::DrawString(10, 70, Util::StringFormat("スポットライトの向き: {%f, %f, %f}", lightVec.x, lightVec.y, lightVec.z), { 1, 1, 1, 1 }, "", 20);
	SimpleDrawer::DrawString(10, 110, Util::StringFormat("スポットライトの減衰係数: {%f, %f, %f}", lightAtten.x, lightAtten.y, lightAtten.z), { 1, 1, 1, 1 }, "", 20);
	SimpleDrawer::DrawString(10, 150, Util::StringFormat("スポットライトの減衰角度: {%f, %f}", lightFactorAngle.x, lightFactorAngle.y), { 1, 1, 1, 1 }, "", 20);
}