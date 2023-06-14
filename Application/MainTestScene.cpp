#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>

MainTestScene::MainTestScene()
{
	sphere = ModelObj(Model::LoadWithAIL("./Resources/Model/", "Sphere.obj", "Sphere"));

	sphere.mTransform.position = { 0, 0, 0 };
	sphere.mTransform.UpdateMatrix();

	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));

	sphere2.mTransform.position = { 0.5f, 0, -1 };
	sphere2.mTransform.UpdateMatrix();

	sphere2.mTuneMaterial.mColor.a = 0.6f;

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
	light.Update();
	camera.Update();

	timer += TimeManager::deltaTime;
	if (timer >= 0.01f) {
		timer = 0;
		for (int32_t i = 0; i < 10; i++) {
			TestObj obj{};
			obj.obj = ModelObj("Sphere");
			obj.speed = Vector3(Util::GetRand(-1.0f, 1.0f), Util::GetRand(-1.0f, 1.0f), Util::GetRand(-1.0f, 1.0f));
			obj.timer = 0;
			testObjList.push_back(obj);
		}
	}

	for (auto itr = testObjList.begin(); itr != testObjList.end();) {
		TestObj& obj = *itr;
		obj.timer += TimeManager::deltaTime;
		if (obj.timer >= 5) {
			itr = testObjList.erase(itr);
			continue;
		}
		obj.obj.mTransform.position += obj.speed;
		obj.obj.mTransform.UpdateMatrix();
		obj.obj.TransferBuffer(camera.mViewProjection);
		itr++;
	}

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

	for (auto itr = testObjList.begin(); itr != testObjList.end(); itr++) {
		TestObj& obj = *itr;
		obj.obj.Draw();
	}

	SimpleDrawer::DrawString(0, 0, 0, Util::StringFormat("Count:%d", testObjList.size()), { 1, 1, 1, 1 }, "", 20);

	for (int32_t i = 0; i < 10; i++) {
		//SimpleDrawer::DrawCircle(Util::GetRand(0, RWindow::GetWidth()), Util::GetRand(0, RWindow::GetHeight()), 5, Color(1, 0, 1, 1));
	}
}