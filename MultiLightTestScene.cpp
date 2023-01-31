#include "MultiLightTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"

MultiLightTestScene::MultiLightTestScene()
{
	controlDesc = Sprite(TextDrawer::CreateStringTexture("ƒ}ƒEƒX:Ž‹ü, WASD:ˆÚ“®, Space:ã¸, LShift:‰º~", "", 32), { 0, 1 });
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
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void MultiLightTestScene::Update()
{
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

void MultiLightTestScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());
	obj.DrawCommands();
	obj2.DrawCommands();
	ground.DrawCommands();

	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	controlDesc.DrawCommands();
}