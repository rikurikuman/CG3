#include <Windows.h>
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
#include "RWindow.h"
#include "RDirectX.h"
#include "RInput.h"
#include "Transform.h"
#include "Texture.h"
#include "Sprite.h"
#include "RConstBuffer.h"
#include "ViewProjection.h"
#include "Image3D.h"
#include "Cube.h"
#include "Matrix4.h"
#include "Util.h"
#include "Model.h"
#include "ModelObj.h"
#include "DebugCamera.h"
#include "BillboardImage.h"
#include "TextDrawer.h"
#include "SphereCollision.h"
#include "Raycast.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "RAudio.h"
#include "SceneManager.h"
#include "MainTestScene.h"
#include "MultiLightTestScene.h"
#include "PointLightTestScene.h"
#include "SpotLightTestScene.h"
#include "SRBuffer.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include "RImGui.h"
#include "SimpleSceneTransition.h"

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

	RAudio::Load("Resources/Sound/start.wav", "Start");
	RAudio::Load("Resources/Sound/countdown.wav", "Countdown");
	RAudio::Load("Resources/Sound/runstart.wav", "RunStart");
	RAudio::Load("Resources/Sound/lap.wav", "Lap");
	RAudio::Load("Resources/Sound/finish.wav", "Finish");

	RAudio::Load("Resources/Sound/engine.wav", "Engine");
	RAudio::Load("Resources/Sound/charge.wav", "Charge");
	RAudio::Load("Resources/Sound/chargemax.wav", "ChargeMax");
	RAudio::Load("Resources/Sound/dash.wav", "Dash");

	RAudio::Load("Resources/Sound/bgm_title.wav", "BGM_Title");
	RAudio::Load("Resources/Sound/bgm_game.wav", "BGM_Game");
	RAudio::Load("Resources/Sound/bgm_result.wav", "BGM_Result");

	TextureManager::Load("Resources/loadingMark.png", "LoadingMark");

	SceneManager::Set<MainTestScene>();

	DebugCamera camera({ 0, 0, -10 });
	Sprite scDesc = Sprite(TextDrawer::CreateStringTexture("数字の1, 2, 3, 4キーでシーン切り替え", "", 32, "scDesc"), {0, 0});
	scDesc.transform.position = { 0, 0, 0 };
	scDesc.transform.UpdateMatrix();
	scDesc.TransferBuffer();

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

		if (!SceneManager::IsSceneChanging()) {
			if (RInput::GetKeyDown(DIK_1)) {
				SceneManager::Change<MainTestScene, SimpleSceneTransition>();
			}
			else if (RInput::GetKeyDown(DIK_2)) {
				SceneManager::Change<MultiLightTestScene, SimpleSceneTransition>();
			}
			else if (RInput::GetKeyDown(DIK_3)) {
				SceneManager::Change<PointLightTestScene, SimpleSceneTransition>();
			}
			else if (RInput::GetKeyDown(DIK_4)) {
				SceneManager::Change<SpotLightTestScene, SimpleSceneTransition>();
			}
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

		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());
		RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
		scDesc.DrawCommands();

		if (Util::debugBool) {
			SimpleDrawer::DrawString(100, 80, Util::StringFormat("FPS: %f", TimeManager::fps));
			SimpleDrawer::DrawString(100, 100, Util::StringFormat("%f, %f", RWindow::GetMousePos().x, RWindow::GetMousePos().y));
		}

		static bool demoWindow = true;
		ImGui::ShowDemoWindow(&demoWindow);

		RImGui::Render();

		//リソースバリアを表示に戻す
		RDirectX::PostDraw();

		TimeManager::Update();

		if (RInput::GetKey(DIK_ESCAPE)) {
			break;
		}
	}

	SceneManager::GameEnd();
	RAudio::Final();
	RImGui::Final();

	return 0;
}