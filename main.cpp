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

	//���f���f�[�^�̓ǂݍ���
	Model::Load("Resources/Model/", "Cube.obj", "Cube");

	SceneManager::Set<GameScene>();

	DebugCamera camera({ 0, 0, -10 });

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

		//�ȉ��`�揈��
		RDirectX::SetBackBufferToRenderTarget();

		//��ʃN���A�`
		RDirectX::ClearRenderTarget(Color(0.0f, 0.0f, 0.0f, 1.0f));
		
		//�[�x�l���N���A
		RDirectX::ClearDepthStencil();

		//�`��R�}���h
		//�r���[�|�[�g�ݒ�R�}���h
		RDirectX::GetInstance()->cmdList->RSSetViewports(1, &viewport);

		//�V�U�[��`
		RDirectX::GetInstance()->cmdList->RSSetScissorRects(1, &scissorRect);

		//�v���~�e�B�u�`��ݒ�
		RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SRV�q�[�v�̐ݒ�R�}���h
		ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
		RDirectX::GetInstance()->cmdList->SetDescriptorHeaps(1, &_heap);

		RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(RDirectX::GetInstance()->rootSignature.ptr.Get());
		RDirectX::GetInstance()->cmdList->SetPipelineState(RDirectX::GetInstance()->pipelineState.ptr.Get());

		//�`��R�}���h
		
		SceneManager::Draw();

		if (Util::debugBool) {
			SimpleDrawer::DrawString(100, 80, Util::StringFormat("FPS: %f", TimeManager::fps));
			SimpleDrawer::DrawString(100, 100, Util::StringFormat("%f, %f", RWindow::GetMousePos().x, RWindow::GetMousePos().y));
		}

		//���\�[�X�o���A��\���ɖ߂�
		RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

		RDirectX::RunDraw();

		TimeManager::Update();

		if (RInput::GetKey(DIK_ESCAPE)) {
			break;
		}
	}

	SceneManager::GameEnd();
	RAudio::Final();

	return 0;
}