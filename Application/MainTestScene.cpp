#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>

MainTestScene::MainTestScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));

	sphere.transform.position = { 0, 0, 0 };
	sphere.transform.UpdateMatrix();

	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));

	sphere2.transform.position = { 0.5f, 0, -1 };
	sphere2.transform.UpdateMatrix();

	sphere2.tuneMaterial.color.a = 0.6f;

	sprite.SetTexture("");
	sprite.SetAnchor({ 0, 0 });
	sprite.transform.position = { 100, 100, 0 };
	sprite.transform.UpdateMatrix();
	sprite.material.color = { 1, 1, 1, 1 };

	sprite2.SetTexture("");
	sprite2.SetAnchor({ 0, 0 });
	sprite2.transform.position = { 120, 120, 0 };
	sprite2.transform.UpdateMatrix();
	sprite2.material.color = { 1, 0, 0, 0.5f };

	camera.viewProjection.eye = { 0, 0, -10 };
	camera.viewProjection.target = { 0, 0, 0 };
	camera.viewProjection.UpdateMatrix();
}

void MainTestScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
	PostEffect::InitPipeline();
	hoge.SetTexture("RenderTargetTex_hoge");
}

void MainTestScene::Update()
{
	light.Update();
	camera.Update();

	sphere.TransferBuffer(camera.viewProjection);
	sphere2.TransferBuffer(camera.viewProjection);
	sprite.TransferBuffer();
	sprite2.TransferBuffer();
	hoge.TransferBuffer();
}

void MainTestScene::Draw()
{
	sphere.Draw();
	sphere2.Draw();

	sprite.Draw();
	sprite2.Draw();

	/*Renderer::SetToBackBuffer();
	Renderer::SetRootSignature(PostEffect::rootSignature);
	Renderer::SetPipeline(PostEffect::pipelineState);
	hoge.TransferBuffer();
	hoge.DrawCommands();

	/*std::vector<RootData> rootData{
		{TextureManager::Get(hoge.texture).gpuHandle},
		{D3D12_ROOT_PARAMETER_TYPE_CBV, hoge.materialBuff.constBuff->GetGPUVirtualAddress()}
	};
	Renderer::DrawCall("TestRenderStage", &hoge.vertBuff.view, &hoge.indexBuff.view, 6, rootData);*/
}