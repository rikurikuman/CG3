#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>

MainTestScene::MainTestScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/", "Skydome.obj", "Skydome", true));
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));

	sphere.mTransform.position = { 0, 0, 0 };
	sphere.mTransform.UpdateMatrix();

	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));

	sphere2.mTransform.position = { 1.5f, 0, -1 };
	sphere2.mTransform.UpdateMatrix();

	cube = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));
	cube.mTransform.position = { 0, -5, 0 };
	cube.mTransform.scale = { 20, 1, 20 };
	cube.mTransform.UpdateMatrix();
	cube.mTuneMaterial.mColor = { 1, 0, 0, 1 };

	sprite.SetTexture("");
	sprite.SetAnchor({ 0, 0 });
	sprite.mTransform.position = { 100, 100, 0 };
	sprite.mTransform.UpdateMatrix();
	sprite.mMaterial.mColor = { 1, 1, 1, 1 };

	sprite2.SetTexture("");
	sprite2.SetAnchor({ 0, 0 });
	sprite2.mTransform.position = { 120, 120, 0 };
	sprite2.mTransform.UpdateMatrix();
	sprite2.mMaterial.mColor = { 1, 0, 0, 0.5f };

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void MainTestScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void MainTestScene::Update()
{
	{
		ImGui::SetNextWindowSize({ 400, 270 });
		ImGui::SetNextWindowPos({ 800, 100 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Scene", NULL, window_flags);

		if(ImGui::Button("Reset")) {

		}

		ImGui::Separator();

		ImGui::Text("MultiRenderTest");
		ImGui::Checkbox("Enable##MultiRender", &useMultiRender);
		
		ImGui::End();
	}

	light.Update();
	camera.Update();

	skydome.TransferBuffer(camera.mViewProjection);
	sphere.TransferBuffer(camera.mViewProjection);
	sphere2.TransferBuffer(camera.mViewProjection);
	cube.TransferBuffer(camera.mViewProjection);
	sprite.TransferBuffer();
	sprite2.TransferBuffer();
}

void MainTestScene::Draw()
{
	skydome.Draw();
	sphere.Draw();
	sphere2.Draw();
	cube.Draw();

	SimpleDrawer::DrawBox(100, 100, 200, 200, 0, { 1, 1, 1, 1 }, true);
	SimpleDrawer::DrawBox(120, 120, 220, 220, 0, { 1, 0, 0, 0.5f }, true);

	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 30, 710, 5, 0, { 1, 0, 0, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 20, 710, 5, 0, { 0, 1, 0, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 10, 710, 5, 0, { 0, 0, 1, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 60, 710, 5, 0, { 1, 0, 0, 1 }, false);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 50, 710, 5, 0, { 0, 1, 0, 1 }, false);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 40, 710, 5, 0, { 0, 0, 1, 1 }, false);
	SimpleDrawer::DrawBox(RWindow::GetWidth() - 80, 705, RWindow::GetWidth() - 70, 715, 0, { 1, 0, 0, 1 }, false, 2);

	if (useMultiRender) multiRenderTest.Draw();
}