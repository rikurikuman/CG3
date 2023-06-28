#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>

MainTestScene::MainTestScene()
{
	levelData = LevelLoader::Load("Resources/untitled.json");

	for (LevelLoader::LevelObject& obj : levelData.objects) {
		ConstructObjectFromLevelData(obj);
	}
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
}

void MainTestScene::Draw()
{
	for (ModelObj& obj : objects) {
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
	}
}

void MainTestScene::ConstructObjectFromLevelData(LevelLoader::LevelObject& obj)
{
	if (obj.type == "MESH") {
		objects.emplace_back("");
		ModelObj& modelObj = objects.back();

		if (obj.hasFileName) {
			modelObj.mModel = ModelManager::Get(obj.fileName);
		}
		if (obj.hasTransform) {
			modelObj.mTransform.position = obj.transform.translation;
			modelObj.mTransform.rotation = obj.transform.rotation;
			modelObj.mTransform.scale = obj.transform.scaling;
			modelObj.mTransform.UpdateMatrix();
		}
	}

	if (!obj.children.empty()) {
		for (LevelLoader::LevelObject child : obj.children) {
			ConstructObjectFromLevelData(child);
		}
	}
}
