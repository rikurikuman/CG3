#include "Sprite.h"
#include <d3d12.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"

using namespace Microsoft::WRL;

Sprite::Sprite(TextureHandle texture, Vector2 anchor)
{
	this->texture = texture;

	//�T�C�Y���Z�b�g����
	
	this->size.x = (float)TextureManager::Get(texture).resource->GetDesc().Width;
	this->size.y = (float)TextureManager::Get(texture).resource->GetDesc().Height;

	//�A���J�[�|�C���g���Z�b�g����
	this->anchor = anchor;

	Init();
}

void Sprite::Init()
{
	//���_�f�[�^
	VertexPNU vertices[] = {
		{{ -anchor.x * size.x, (1 - anchor.y) * size.y, 0.0f}, {}, {0.0f, 1.0f}}, //����
		{{ -anchor.x * size.x, -anchor.y * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //����
		{{ (1 - anchor.x) * size.x, (1 - anchor.y) * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //�E��
		{{ (1 - anchor.x) * size.x, -anchor.y * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	VertexPNU::CalcNormalVec(vertices, indices, _countof(indices));

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));
}

void Sprite::TransferBuffer()
{
	material.Transfer(materialBuff.constMap);
	transform.Transfer(transformBuff.constMap);

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);
	
	viewProjectionBuff.constMap->matrix = matProjection;
}

void Sprite::DrawCommands()
{
	//�p�C�v���C���Z�b�g
	RDirectX::GetInstance()->cmdList->SetPipelineState(SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(SpriteManager::GetInstance()->GetRootSignature().ptr.Get());

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &vertBuff.view);

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->IASetIndexBuffer(&indexBuff.view);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(texture).gpuHandle);

	TransferBuffer();

	//�`��R�}���h
	RDirectX::GetInstance()->cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void SpriteManager::Init()
{
	rootSignature = RDirectX::GetInstance()->rootSignature;

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
	rootSignature.desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	rootSignature.Create();

	pipelineState = RDirectX::GetInstance()->pipelineState;

	pipelineState.desc.VS = Shader("./Shader/SpriteVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/SpritePS.hlsl", "main", "ps_5_0");

	// ���X�^���C�U�̐ݒ�
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;
	pipelineState.desc.BlendState.AlphaToCoverageEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}
