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
#include "GameScene.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;
using namespace DirectX;

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	//WindowsAPI
	RWindow::SetWindowName(L"Gradiu3D");
	RWindow::Init();

	//DirectX
	RDirectX::Init();

	//DirectInput
	RInput::Init();

	//XAudio2
	RAudio::Init();

	TimeManager::Init();

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(RDirectX::GetInstance()->device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
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
	Model::Load("Resources/Model/VicViper/", "VicViper.obj", "VicViper");

	//音声データの読み込み
	RAudio::Load("Resources/Sound/Music_Main.wav", "MusicMain");
	RAudio::Load("Resources/Sound/Music_Boss.wav", "MusicBoss");
	RAudio::Load("Resources/Sound/Music_GameOver.wav", "MusicGameOver");
	RAudio::Load("Resources/Sound/CursorMove.wav", "CursorMove");
	RAudio::Load("Resources/Sound/Shot.wav", "Shot");
	RAudio::Load("Resources/Sound/Laser.wav", "Laser");
	RAudio::Load("Resources/Sound/Defeat.wav", "Defeat");
	RAudio::Load("Resources/Sound/ItemPickup.wav", "ItemPickup");
	RAudio::Load("Resources/Sound/ItemUse.wav", "ItemUse");
	RAudio::Load("Resources/Sound/EnemyBreak.wav", "EnemyBreak");
	RAudio::Load("Resources/Sound/BossBreak.wav", "BossBreak");
	RAudio::Load("Resources/Sound/Start.wav", "Start");

	TextureManager::Load("Resources/shutter.png", "shutter");

	SceneManager::Set<GameScene>();

	//////////////////////////////////////

	while (true) {
		RWindow::ProcessMessage();

		if (RWindow::GetInstance()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		if (Util::instanceof<DebugCamera>(*Camera::nowCamera)
			&& GetForegroundWindow() == RWindow::GetWindowHandle()) {
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

		SceneManager::Update();

		//以下描画処理
		RDirectX::SetBackBufferToRenderTarget();

		//画面クリア～
		RDirectX::ClearRenderTarget(Color(0.0f, 0.0f, 0.0f, 1.0f));
		
		//深度値もクリア
		RDirectX::ClearDepthStencil();

		//描画コマンド
		//ビューポート設定コマンド
		RDirectX::GetInstance()->cmdList->RSSetViewports(1, &viewport);

		//シザー矩形
		RDirectX::GetInstance()->cmdList->RSSetScissorRects(1, &scissorRect);

		//プリミティブ形状設定
		RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SRVヒープの設定コマンド
		ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
		RDirectX::GetInstance()->cmdList->SetDescriptorHeaps(1, &_heap);

		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());

		//描画コマンド
		
		SceneManager::Draw();

		if (Util::debugBool) {
			SimpleDrawer::DrawString(100, 80, Util::StringFormat("FPS: %f", TimeManager::fps));
			SimpleDrawer::DrawString(100, 100, Util::StringFormat("%f, %f", RWindow::GetMousePos().x, RWindow::GetMousePos().y));
		}

		//リソースバリアを表示に戻す
		RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

		RDirectX::RunDraw();

		TimeManager::Update();

		if (RInput::GetKey(DIK_ESCAPE)) {
			break;
		}
	}

	RAudio::Final();

	return 0;
}