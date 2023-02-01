#include <Windows.h>
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
#include "RWindow.h"
#include "RDirectX.h"
#include "RInput.h"
#include "Texture.h"
#include "ViewProjection.h"
#include "Util.h"
#include "Model.h"
#include "DebugCamera.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "RAudio.h"
#include "SceneManager.h"
#include "SimpleSceneTransition.h"
#include "RImGui.h"
#include "SpriteScene.h"
#include "ModelScene.h"
#include "SoundScene.h"
#include "MainTestScene.h"
#include "MultiLightTestScene.h"
#include "PointLightTestScene.h"
#include "SpotLightTestScene.h"
#include "VerySlowLoadScene.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

using namespace std;
using namespace DirectX;

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	//WindowsAPI
	RWindow::SetWindowName(L"RKEngine");
	RWindow::Init();

	//DirectX
	RDirectX::Init();

	//ImGui
	RImGui::Init();

	//DirectInput
	RInput::Init();

	//XAudio2
	RAudio::Init();

	TimeManager::Init();

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(RDirectX::GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	}
#endif

	///////////////////

	D3D12_VIEWPORT viewport{};
	viewport.Width = WIN_WIDTH;
	viewport.Height = WIN_HEIGHT;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + WIN_WIDTH;
	scissorRect.top = 0;
	scissorRect.bottom = scissorRect.top + WIN_HEIGHT;

	//モデルデータの読み込み
	Model::Load("Resources/Model/", "Cube.obj", "Cube");

	TextureManager::Load("Resources/loadingMark.png", "LoadingMark");

	SceneManager::Set<SpriteScene>();

	DebugCamera camera({ 0, 0, -10 });

	//////////////////////////////////////

	while (true) {
		RWindow::ProcessMessage();

		if (RWindow::GetInstance()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		RImGui::NewFrame();

		if (Util::instanceof<DebugCamera>(*Camera::nowCamera)
			&& GetForegroundWindow() == RWindow::GetWindowHandle()
			&& !dynamic_cast<DebugCamera*>(Camera::nowCamera)->freeFlag) {
			RWindow::SetMouseHideFlag(true);
			RWindow::SetMousePos(RWindow::GetWidth() / 2, RWindow::GetHeight() / 2);
		}
		else {
			RWindow::SetMouseHideFlag(false);
		}

		if (RInput::GetKeyDown(DIK_F5)) {
			Util::debugBool = !Util::debugBool;
		}

		RInput::Update();

		//べんり！
		{
			ImGui::SetNextWindowSize({ 400, 300 });

			ImGuiWindowFlags window_flags = 0;
			 window_flags |= ImGuiWindowFlags_NoResize;
			ImGui::Begin("Debug", NULL, window_flags);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::NewLine();
			ImGui::Text("SceneManager");
			static int sceneNum = 0;
			const char* scenes[] = {"Sprite", "Model", "Sound", "DirectionalLight", "PointLight", "SpotLight", "VerySlowLoad"};
			ImGui::Combo("##SceneNumCombo", &sceneNum, scenes, IM_ARRAYSIZE(scenes));
			ImGui::SameLine();
			if (ImGui::Button("Go!!!")) {
				if (!SceneManager::IsSceneChanging()) {
					switch (sceneNum) {
					case 0:
						SceneManager::Change<SpriteScene, SimpleSceneTransition>();
						break;
					case 1:
						SceneManager::Change<ModelScene, SimpleSceneTransition>();
						break;
					case 2:
						SceneManager::Change<SoundScene, SimpleSceneTransition>();
						break;
					case 3:
						SceneManager::Change<MultiLightTestScene, SimpleSceneTransition>();
						break;
					case 4:
						SceneManager::Change<PointLightTestScene, SimpleSceneTransition>();
						break;
					case 5:
						SceneManager::Change<SpotLightTestScene, SimpleSceneTransition>();
						break;
					case 6:
						SceneManager::Change<VerySlowLoadScene, SimpleSceneTransition>();
						break;
					}
				}
			}

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::Text("DebugCamera Control");
			ImGui::Text("* Valid only in available scenes.");
			ImGui::Text("Left Control Key : Enable / Disable");
			ImGui::Text("WASD Key : Move");
			ImGui::Text("Space Key : Up");
			ImGui::Text("LShift Key : Down");
			ImGui::Text("Mouse : Direction");
			ImGui::End();
		}

		SceneManager::Update();

		//以下描画処理
		RDirectX::PreDraw();

		//画面クリア～
		RDirectX::ClearRenderTarget(Color(0.0f, 0.0f, 0.0f, 1.0f));
		
		//深度値もクリア
		RDirectX::ClearDepthStencil();

		//ビューポート設定コマンド
		RDirectX::GetCommandList()->RSSetViewports(1, &viewport);

		//シザー矩形
		RDirectX::GetCommandList()->RSSetScissorRects(1, &scissorRect);

		//描画コマンド
		
		SceneManager::Draw();

		RImGui::Render();

		//リソースバリアを表示に戻す
		RDirectX::PostDraw();

		TimeManager::Update();

		if (RInput::GetKey(DIK_ESCAPE)) {
			break;
		}
	}

	SceneManager::Finalize();
	RAudio::Finalize();
	RImGui::Finalize();

	return 0;
}