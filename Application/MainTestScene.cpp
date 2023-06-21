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
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));

	sphere.mTransform.position = { 0, 0, 0 };
	sphere.mTransform.UpdateMatrix();

	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));

	sphere2.mTransform.position = { 1.5f, 0, -1 };
	sphere2.mTransform.UpdateMatrix();

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
		ImGui::SetNextWindowSize({ 400, 450 });
		ImGui::SetNextWindowPos({ 800, 100 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Scene", NULL, window_flags);

		if(ImGui::Button("Reset")) {
			useBloom = true;
			bloom.mSetting.sigma = 0.002f;
		}
		ImGui::Separator();
		ImGui::Text("Bloom");
		ImGui::Checkbox("Enable", &useBloom);
		static int32_t bloomstep = 3;
		const char* steps[] = { "None", "HighLumiExtract", "Blur", "Bloom" };
		ImGui::Combo("BloomStep##SceneNumCombo", &bloomstep, steps, IM_ARRAYSIZE(steps));
		switch (bloomstep) {
		case 0:
			bloom.mLevel = 0;
			break;
		case 1:
			bloom.mLevel = 1;
			break;
		case 2:
			bloom.mLevel = 2;
			break;
		case 3:
			bloom.mLevel = 3;
			break;
		}
		ImGui::SliderFloat("Sigma", &bloom.mSetting.sigma, 0, 0.05f);
		ImGui::End();
	}

	light.Update();
	camera.Update();

	sphere.TransferBuffer(camera.mViewProjection);
	sphere2.TransferBuffer(camera.mViewProjection);
	sprite.TransferBuffer();
	sprite2.TransferBuffer();
}

void MainTestScene::Draw()
{
	sphere.Draw();
	sphere2.Draw();

	sprite.Draw();
	sprite2.Draw();

	if(useBloom) bloom.Draw();
}