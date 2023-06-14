#include "SoundScene.h"
#include "RImGui.h"
#include "Camera.h"
#include "LightGroup.h"

SoundScene::SoundScene()
{
	handle = RAudio::Load("./Resources/engine.wav", "Engine");
}

void SoundScene::Init()
{
	Camera::sNowCamera = nullptr;
	LightGroup::sNowLight = nullptr;
}

void SoundScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 150 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Sound Control", NULL, window_flags);

		ImGui::PushButtonRepeat(true);
		if (ImGui::Button("Play")) {
			RAudio::Play(handle, volume, pitch);
		}
		ImGui::PopButtonRepeat();

		ImGui::Checkbox("AutoPlay", &autoPlay);
		ImGui::DragFloat("Volume", &volume, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Pitch", &pitch, 0.01f, 0.0f, 2.0f);

		ImGui::End();
	}

	if (autoPlay) {
		RAudio::Play(handle, volume, pitch);
	}
}
