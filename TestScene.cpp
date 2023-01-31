#include "TestScene.h"
#include "RInput.h"

TestScene::TestScene()
{
	obj = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "NonSmoothVic", false));
	obj.transform.position = { -1, 0, 0 };
	obj.transform.scale = { 0.2f, 0.2f, 0.2f };
	obj.transform.UpdateMatrix();

	obj2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "SmoothVic", true));
	obj2.transform.position = { 1, 0, 0 };
	obj2.transform.scale = { 0.2f, 0.2f, 0.2f };
	obj2.transform.UpdateMatrix();

	sp.SetAnchor({0, 0});

	light.lightVec = { 1, -1, 1 };
	light.TransferBuffer();
}

void TestScene::Init()
{
	Camera::nowCamera = &camera;
	Light::nowLight = &light;
}

void TestScene::Update()
{
	camera.Update();

	obj.transform.rotation.z += Util::AngleToRadian(1);
	obj2.transform.rotation.z += Util::AngleToRadian(1);

	obj.transform.UpdateMatrix();
	obj2.transform.UpdateMatrix();

	if (RInput::GetKeyDown(DIK_1)) {
		sp.SetTexture("");
	}
	else if (RInput::GetKeyDown(DIK_2)) {
		sp.SetTexture(TextureManager::Load("./Resources/conflict.jpg", "conflict"));
	}
	else if (RInput::GetKeyDown(DIK_3)) {
		sp.SetTexture(TextureManager::Load("./Resources/conflict.jpg", "conflict"));
		sp.SetTexRect(400, 400, 100, 100);
	}
	else if (RInput::GetKeyDown(DIK_4)) {
		sp.SetTexture("sadhoiashdoasidhosauhfafheuhroiadfhqilauemadiuvfuad");
	}

	obj.TransferBuffer(Camera::nowCamera->viewProjection);
	obj2.TransferBuffer(Camera::nowCamera->viewProjection);
	sp.TransferBuffer();
}

void TestScene::Draw()
{
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());
	obj.DrawCommands();
	obj2.DrawCommands();

	RDirectX::GetCommandList()->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());
	sp.DrawCommands();
}
