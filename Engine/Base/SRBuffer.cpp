#include "SRBuffer.h"
#include "RDirectX.h"

SRBufferAllocator* SRBufferAllocator::GetInstance()
{
	static SRBufferAllocator instance;
	return &instance;
}

UINT64 Align(UINT64 location, UINT64 align) {
	if ((align == 0) || (align & (align - 1))) {
		return 0;
	}
	return (location + (align - 1)) & ~(align - 1);
}

UINT8* SRBufferAllocator::Alloc(UINT64 needSize, UINT align)
{
	SRBufferAllocator* instance = GetInstance();

	//�m�ۂ����̈�̐擪�A�h���X���w���|�C���^
	UINT8* newLoc = nullptr;

	//�󂫗̈悩��~�����T�C�Y�����܂�̈��T��
	for (FreeRegion reg : instance->freeRegions) {

		//�v�����E�ɃA���C�����g����
		UINT8* alignedLoc = reinterpret_cast<UINT8*>(Align(reinterpret_cast<UINT64>(reg.pBegin), align));

		//�A���C�����g�Ɏ��s����悤�Ȃ�null�̂܂ܕԂ�
		if (alignedLoc == nullptr) return nullptr;

		//�A���C�����g���ꂽ��̃A�h���X����v���T�C�Y���m�ۂł��邩�m�F
		if (reg.pEnd < alignedLoc || UINT64(reg.pEnd - alignedLoc) < needSize) {
			continue; //�ł��Ȃ��Ȃ�ʂ̋󂫗̈�ł�����x
		}

		//�ł���Ȃ�̗p
		newLoc = alignedLoc;
		break;
	}

	//�m�ۂł��Ȃ�������null�̂܂ܕԂ�
	if (newLoc == nullptr) return nullptr;

	//�󂫗̈����ҏW����
	//�m�ۗ̈�̐擪�A�h���X���܂܂�Ă���󂫗̈��T��
	for (auto itr = instance->freeRegions.begin(); itr != instance->freeRegions.end(); itr++) {
		FreeRegion& reg = *itr;
		if (newLoc < reg.pBegin || reg.pEnd < newLoc) {
			continue;
		}

		//����΁A�󂫗̈�̐擪����m�ۗ̈�̐擪�܂ł�V���ȋ󂫗̈�Ƃ��Ēǉ�����
		if (UINT64(newLoc - reg.pBegin) != 0) {
			itr = instance->freeRegions.emplace(itr, reg.pBegin, newLoc - 1);
			itr++;
		}

		//����΁A�m�ۗ̈�̖�������󂫗̈�̖����܂ł�V���ȋ󂫗̈�Ƃ��Ēǉ�����
		if (UINT64(reg.pEnd - (newLoc + needSize)) != 0) {
			itr = instance->freeRegions.emplace(itr++, newLoc + needSize, reg.pEnd);
			itr++;
		}

		//�󂫗̈�폜
		instance->freeRegions.erase(itr);
		break;
	}

	return newLoc;
}

SRBufferAllocator::SRBufferAllocator() {

	size_t bufferSize = (1048576 + 0xff) & ~0xff; //256�o�C�g�A���C�������g

	// �m��
	HRESULT result;

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = bufferSize; 
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer)
	);
	assert(SUCCEEDED(result));

	result = buffer->Map(0, nullptr, (void**)&pBufferBegin); //�}�b�s���O
	assert(SUCCEEDED(result));

	pBufferEnd = pBufferBegin + bufferSize;

	freeRegions.emplace_back(pBufferBegin, pBufferEnd);
}