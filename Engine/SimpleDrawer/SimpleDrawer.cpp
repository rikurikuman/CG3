#include "SimpleDrawer.h"
#include "RDirectX.h"
#include "RWindow.h"
#include "TextDrawer.h"
#include <chrono>

using namespace std;

void SimpleDrawer::DrawBox(float x1, float y1, float x2, float y2, Color color, bool fillFlag)
{
	SimpleDrawer* instance = GetInstance();

	shared_ptr<DrawBoxInfo> info = make_shared<DrawBoxInfo>();
	info->start = { x1, y1 };
	info->end = { x2, y2 };

	//���_�f�[�^
	VertexP vertices[] = {
		{{ x1, y1, 0.0f }}, //����
		{{ x1, y2, 0.0f }}, //����
		{{ x2, y1, 0.0f }}, //�E��
		{{ x2, y2, 0.0f }}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	UINT indices[] = {
		0, 2, 1,
		1, 2, 3
	};

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);

	info->vertBuff.Init(vertices, _countof(vertices));
	info->indexBuff.Init(indices, _countof(indices));
	*info->colorBuff.constMap = color;
	info->viewProjectionBuff.constMap->matrix = matProjection;

	instance->infoList.push_back(info);
	
	info->pipelineState = instance->pipelineState;
	if (fillFlag) {
		info->pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	}
	else {
		info->pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	info->pipelineState.Create();

	RDirectX::GetCommandList()->SetPipelineState(info->pipelineState.ptr.Get());
	RDirectX::GetCommandList()->SetGraphicsRootSignature(instance->rootSignature.ptr.Get());

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &info->vertBuff.view);

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->IASetIndexBuffer(&info->indexBuff.view);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(0, info->colorBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(1, info->viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//�`��R�}���h
	RDirectX::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void SimpleDrawer::DrawString(float x, float y, std::string text, Color color, std::string fontTypeFace, UINT fontSize, Vector2 anchor)
{
	SimpleDrawer* instance = GetInstance();
	shared_ptr<DrawStringInfo> info = make_shared<DrawStringInfo>();
	TextureHandle tex = TextDrawer::CreateStringTexture(text, fontTypeFace, fontSize);
	info->sprite = Sprite(tex, anchor);
	info->sprite.transform.position = { x, y, 0 };
	info->sprite.transform.UpdateMatrix();
	info->sprite.material.color = color;
	info->sprite.TransferBuffer();

	info->sprite.Draw();

	TextureManager::UnRegisterAtEndFrame(tex);
	instance->infoList.push_back(info);
}

void SimpleDrawer::Init()
{
	rootSignature = RDirectX::GetDefRootSignature();

	// ���[�g�p�����[�^�̐ݒ�
	RootParamaters rootParams(2);
	//�萔�o�b�t�@0��(Color)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@2��(ViewProjection)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[1].Descriptor.ShaderRegister = 1; //�萔�o�b�t�@�ԍ�
	rootParams[1].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����

	rootSignature.desc.RootParamaters = rootParams;
	rootSignature.Create();

	pipelineState = RDirectX::GetDefPipeline();

	pipelineState.desc.VS = Shader("./Shader/SimpleVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/SimplePS.hlsl", "main", "ps_5_0");

	// ���X�^���C�U�̐ݒ�
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;

	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();

	//DrawString�p
	rootSignatureForString = SpriteManager::GetInstance()->GetRootSignature();

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //���j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f; //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //�s�N�Z���V�F�[�_�[���炾��������
	rootSignatureForString.desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	rootSignatureForString.Create();

	pipelineStateForString = SpriteManager::GetInstance()->GetGraphicsPipeline();
	pipelineStateForString.desc.pRootSignature = rootSignatureForString.ptr.Get();
	pipelineStateForString.Create();
}
