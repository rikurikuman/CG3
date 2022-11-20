#include "Particle.h"
#include "Camera.h"
#include "TimeManager.h"

Particle::Particle(RootSignature* root, GraphicsPipeline* pipeline)
	: root(root), pipeline(pipeline) {

	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(VertexInfo));

	//���_�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//���_�o�b�t�@����
	result = RDirectX::GetInstance()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//����͒��_�o�b�t�@�̃}�b�s���O
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	*vertMap = data;

	//���_�o�b�t�@�r���[�̍쐬
	vertBuffView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	vertBuffView.SizeInBytes = dataSize; //���_�o�b�t�@�̃T�C�Y
	vertBuffView.StrideInBytes = sizeof(VertexInfo); //���_��̃T�C�Y
}

void Particle::Update()
{
	aliveTime -= TimeManager::deltaTime;
	if (aliveTime <= 0) {
		alive = false;
		return;
	}

	float t = min(1, aliveTime / maxAliveTime);
	velocity += accel * TimeManager::deltaFrame;
	data.pos += velocity * TimeManager::deltaFrame;
	data.scale = { scale.x * t, scale.y * t };
	data.color = endcolor * (1 - t) + color * t;

	*vertMap = data;

	Matrix4 mat = Camera::nowCamera->viewProjection.view;
	mat = -mat;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	*billboardBuff.constMap = mat;

	viewProjectionBuff.constMap->matrix = Camera::nowCamera->viewProjection.matrix;
}

void Particle::DrawCommands()
{
	//�p�C�v���C���Z�b�g
	RDirectX::GetInstance()->cmdList->SetPipelineState(pipeline->ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(root->ptr.Get());

	RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &vertBuffView);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, billboardBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����(�w�i)
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(texture).gpuHandle);

	//�`��R�}���h
	RDirectX::GetInstance()->cmdList->DrawInstanced(1, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
	RDirectX::GetInstance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}
