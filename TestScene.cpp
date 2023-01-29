#include "TestScene.h"

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

	obj.TransferBuffer(Camera::nowCamera->viewProjection);
	obj2.TransferBuffer(Camera::nowCamera->viewProjection);
}

void TestScene::Draw()
{
	RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());
	obj.DrawCommands();
	obj2.DrawCommands();
}
