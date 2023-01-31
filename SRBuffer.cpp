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

	//確保した領域の先頭アドレスを指すポインタ
	UINT8* newLoc = nullptr;

	//空き領域から欲しいサイズが収まる領域を探す
	for (FreeRegion reg : instance->freeRegions) {

		//要求境界にアライメントする
		UINT8* alignedLoc = reinterpret_cast<UINT8*>(Align(reinterpret_cast<UINT64>(reg.pBegin), align));

		//アライメントに失敗するようならnullのまま返す
		if (alignedLoc == nullptr) return nullptr;

		//アライメントされた後のアドレスから要求サイズが確保できるか確認
		if (reg.pEnd < alignedLoc || UINT64(reg.pEnd - alignedLoc) < needSize) {
			continue; //できないなら別の空き領域でもう一度
		}

		//できるなら採用
		newLoc = alignedLoc;
		break;
	}

	//確保できなかったらnullのまま返す
	if (newLoc == nullptr) return nullptr;

	//空き領域情報を編集する
	//確保領域の先頭アドレスが含まれている空き領域を探す
	for (auto itr = instance->freeRegions.begin(); itr != instance->freeRegions.end(); itr++) {
		FreeRegion& reg = *itr;
		if (newLoc < reg.pBegin || reg.pEnd < newLoc) {
			continue;
		}

		//あれば、空き領域の先頭から確保領域の先頭までを新たな空き領域として追加する
		if (UINT64(newLoc - reg.pBegin) != 0) {
			itr = instance->freeRegions.emplace(itr, reg.pBegin, newLoc - 1);
			itr++;
		}

		//あれば、確保領域の末尾から空き領域の末尾までを新たな空き領域として追加する
		if (UINT64(reg.pEnd - (newLoc + needSize)) != 0) {
			itr = instance->freeRegions.emplace(itr++, newLoc + needSize, reg.pEnd);
			itr++;
		}

		//空き領域削除
		instance->freeRegions.erase(itr);
		break;
	}

	return newLoc;
}

SRBufferAllocator::SRBufferAllocator() {

	size_t bufferSize = (1048576 + 0xff) & ~0xff; //256バイトアラインメント

	// 確保
	HRESULT result;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = bufferSize; 
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer)
	);
	assert(SUCCEEDED(result));

	result = buffer->Map(0, nullptr, (void**)&pBufferBegin); //マッピング
	assert(SUCCEEDED(result));

	pBufferEnd = pBufferBegin + bufferSize;

	freeRegions.emplace_back(pBufferBegin, pBufferEnd);
}