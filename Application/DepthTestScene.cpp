#include "DepthTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>

DepthTestScene::DepthTestScene()
{
	Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true);
	Model::Load("./Resources/Model/", "Cube.obj", "Cube", false);

	objList.push_back({ "Cube", {0, -3, 0}, {0, 0, 0}, {50, 0.5f, 50}, {0.7f, 0, 0, 1 } });
	objList.push_back({ "Sphere", {0, 0, 0}, {0, 0, 0}, {1, 1, 1}, {0, 0, 1, 1} });
	objList.push_back({ "Cube", {1.5f, 0, -1}, {0, 0, 0}, {1, 1, 1} });

	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });
	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });
	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });
	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });
	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });
	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });
	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });
	objList.push_back({ "Cube", {Util::GetRand(-19.0f, 19.0f), 0, Util::GetRand(-19.0f, 19.0f)}, {0, 0, 0}, {1, 10, 1} });

	objList.push_back({ "Sphere", {0, 10, 50}, {0, 0, 0}, {10, 10, 10} });

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	light.SetDirectionalLightVec(0, { 1, -1, -1 });
}

void DepthTestScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void DepthTestScene::Update()
{
	{
		ImGui::SetNextWindowSize({ 400, 220 });
		ImGui::SetNextWindowPos({ 800, 100 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Scene", NULL, window_flags);

		if(ImGui::Button("Reset")) {
			useDepthTest = false;
			depthTest.mSetting.useDepthColor = false;
			depthTest.mSetting.focusDepth = 0.95f;
			depthTest.mSetting.nearFocusWidth = 0.02f;
			depthTest.mSetting.farFocusWidth = 0.04f;
		}

		ImGui::Separator();

		ImGui::Text("DepthTest");
		ImGui::Checkbox("Enable##DepthTest", &useDepthTest);
		ImGui::Checkbox("UseDepthColor##DepthTestAA", (bool*)&depthTest.mSetting.useDepthColor);
		ImGui::SliderFloat("FocusDepth", &depthTest.mSetting.focusDepth, 0, 1);
		ImGui::SliderFloat("NearFocusWidth", &depthTest.mSetting.nearFocusWidth, 0, 1);
		ImGui::SliderFloat("FarFocusWidth", &depthTest.mSetting.farFocusWidth, 0, 1);
		ImGui::End();
	}

	light.Update();
	camera.Update();
}

void DepthTestScene::Draw()
{
	for (TestObj& obj : objList) {
		obj.obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.obj.Draw();
	}

	if (useDepthTest) {
		depthTest.Draw();
	}
}