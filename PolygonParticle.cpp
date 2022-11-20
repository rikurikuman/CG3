#include "PolygonParticle.h"
#include "TimeManager.h"
#include "Camera.h"

PolygonParticle::PolygonParticle(RootSignature* root, GraphicsPipeline* pipeline)
	: root(root), pipeline(pipeline) {

	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * 3);

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

	//���_�o�b�t�@�r���[�̍쐬
	vertBuffView.BufferLocation = vertBuff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	vertBuffView.SizeInBytes = dataSize; //���_�o�b�t�@�̃T�C�Y
	vertBuffView.StrideInBytes = sizeof(VertexPNU); //���_��̃T�C�Y
}

void PolygonParticle::Update()
{
	aliveTime -= TimeManager::deltaTime;
	if (aliveTime <= 0) {
		alive = false;
		return;
	}

	float t = min(1, aliveTime / maxAliveTime);
	velocity += accel * TimeManager::deltaFrame;
	transform.position += velocity;
	transform.UpdateMatrix();
	
	for (int i = 0; i < 3; i++) {
		vertMap[i] = vertices[i];
	}

	transformBuff.constMap->matrix = transform.matrix;
	material.Transfer(materialBuff.constMap);
	viewProjectionBuff.constMap->matrix = Camera::nowCamera->viewProjection.matrix;
}

void PolygonParticle::DrawCommands()
{
	//�p�C�v���C���Z�b�g
	RDirectX::GetInstance()->cmdList->SetPipelineState(pipeline->ptr.Get());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootSignature(root->ptr.Get());

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->IASetVertexBuffers(0, 1, &vertBuffView);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(2, transformBuff.constBuff->GetGPUVirtualAddress());
	RDirectX::GetInstance()->cmdList->SetGraphicsRootConstantBufferView(3, viewProjectionBuff.constBuff->GetGPUVirtualAddress());

	//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����(�w�i)
	RDirectX::GetInstance()->cmdList->SetGraphicsRootDescriptorTable(0, TextureManager::Get(material.texture).gpuHandle);

	//�`��R�}���h
	RDirectX::GetInstance()->cmdList->DrawInstanced(3, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��

}
