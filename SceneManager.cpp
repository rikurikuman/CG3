#include "SceneManager.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include "Util.h"

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

Scene* SceneManager::GetNowScene() {
	return GetInstance()->nowScene.get();
}

bool SceneManager::IsSceneChanging() {
	return !GetInstance()->runningSceneChange.empty();
}

bool SceneManager::CheckGameEndFlag()
{
	return GetInstance()->gameEnd;
}

void SceneManager::GameEnd()
{
	GetInstance()->gameEnd = true;
}

void SceneManager::Update() {
	SceneManager* instance = GetInstance();

	if (!instance->runningSceneChange.empty()) {
		SceneChange& sc = *instance->runningSceneChange.begin();

		if (sc.increment == 0) {
			sc.transition->Close();
			sc.increment++;
		}
		sc.transition->Update();

		if (sc.increment == 1
				&& sc.transition->IsClosed()
				&& sc.future->valid()
				&& sc.future->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			sc.scene = sc.future->get();
			sc.future2 = std::make_shared<std::future<bool>>(std::async(std::launch::async, [&sc] {
				SceneManager* instance = GetInstance();
				std::lock_guard<std::mutex> lock(instance->mutex);
				std::shared_ptr<Scene> scene = std::move(sc.scene);
				std::swap(instance->changeScene, scene);
				return true;
			}));
			sc.increment++;
		}

		if (sc.increment == 2
			&& sc.transition->IsClosed()
			&& sc.future2->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			std::lock_guard<std::mutex> lock(instance->mutex);
			std::swap(instance->nowScene, instance->changeScene);
			instance->nowScene->Init();
			sc.transition->Open();
			sc.increment++;
		}

		if (sc.increment == 3
			&& sc.transition->IsOpened()) {
			instance->runningSceneChange.erase(instance->runningSceneChange.begin());
		}
	}

	if (instance->nowScene != nullptr) {
		instance->nowScene->Update();
	}
}

void SceneManager::Draw() {
	SceneManager* instance = GetInstance();
	if (instance->nowScene != nullptr) {
		instance->nowScene->Draw();
	}
	if (!instance->runningSceneChange.empty()) {
		SceneChange& sc = *instance->runningSceneChange.begin();
		sc.transition->Draw();
	}
}