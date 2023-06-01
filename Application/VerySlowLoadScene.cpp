#include "VerySlowLoadScene.h"
#include <thread>
#include "Camera.h"
#include "LightGroup.h"
#include "SimpleDrawer.h"

VerySlowLoadScene::VerySlowLoadScene()
{
	std::this_thread::sleep_for(std::chrono::seconds(5));
}

void VerySlowLoadScene::Init()
{
	Camera::nowCamera = nullptr;
	LightGroup::nowLight = nullptr;
}

void VerySlowLoadScene::Draw()
{
	SimpleDrawer::DrawString(0, static_cast<float>(RWindow::GetHeight()), 0, "このシーンのコンストラクタは5秒Sleepしています", { 1, 1, 1, 1 }, "", 60, { 0, 1 });
}
