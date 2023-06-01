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
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
}

void MainTestScene::Update()
{
	light.Update();
	camera.Update();
}

void MainTestScene::Draw()
{
	for (ModelObj& obj : objects) {
		obj.TransferBuffer(Camera::nowCamera->viewProjection);
		obj.Draw();
	}
}

void MainTestScene::ConstructObjectFromLevelData(LevelLoader::LevelObject& obj)
{
	if (obj.type == "MESH") {
		objects.emplace_back("");
		ModelObj& modelObj = objects.back();

		if (obj.hasFileName) {
			modelObj.model = ModelManager::Get(obj.fileName);
		}
		if (obj.hasTransform) {
			modelObj.transform.position = obj.transform.translation;
			modelObj.transform.rotation = obj.transform.rotation;
			modelObj.transform.scale = obj.transform.scaling;
			modelObj.transform.UpdateMatrix();
		}
	}

	if (!obj.children.empty()) {
		for (LevelLoader::LevelObject child : obj.children) {
			ConstructObjectFromLevelData(child);
		}
	}
}
