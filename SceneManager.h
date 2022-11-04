#pragma once
#include <memory>
#include <future>
#include <vector>
#include "Scene.h"
#include "SceneTransition.h"

class SceneManager
{
public:
	static SceneManager* GetInstance();
	static Scene* GetNowScene();

	static bool IsSceneChanging();
	static bool CheckGameEndFlag();

	static void GameEnd();
	static void Update();
	static void Draw();

private:
	struct SceneChange {
		std::shared_ptr<std::future<std::shared_ptr<Scene>>> future = nullptr;
		std::shared_ptr<std::future<bool>> future2 = nullptr;
		std::shared_ptr<SceneTransition> transition = nullptr;
		std::shared_ptr<Scene> scene = nullptr;
		int increment = 0;
	};

	std::mutex mutex;
	std::shared_ptr<Scene> nowScene = nullptr;
	std::shared_ptr<Scene> changeScene = nullptr;
	std::vector<SceneChange> runningSceneChange;
	bool gameEnd = false;

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager& a) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

public:
	template<class SceneClassName, class SceneTransitionClassName = SceneTransition>
	static void Change() {
		SceneManager* instance = GetInstance();

		SceneChange sc{};
		sc.future = std::make_shared<std::future<std::shared_ptr<Scene>>>(std::async(std::launch::async, [] {
			SceneManager* instance = GetInstance();
			std::shared_ptr<Scene> newScene = std::make_shared<SceneClassName>();
			return newScene;
		}));

		sc.transition = std::make_shared<SceneTransitionClassName>();
		instance->runningSceneChange.push_back(sc);
	}

	template<typename SceneClassName>
	static void Set() {
		SceneManager* instance = GetInstance();
		instance->runningSceneChange.clear();
		std::unique_ptr<Scene> newScene = std::make_unique<SceneClassName>();
		instance->nowScene = std::move(newScene);
		instance->nowScene->Init();
	}
};