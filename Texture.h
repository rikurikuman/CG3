#pragma once
#include <d3d12.h>
#include <string>
#include <list>
#include <wrl.h>
#include <memory>
#include <map>
#include <mutex>

typedef std::string TextureHandle;

class Texture
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource; //テクスチャのリソース
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(CPU側)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(GPU側)
	UINT heapIndex = -1;
	std::string filePath; //ファイルへのパス

	Texture() {};
	Texture(
		Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
		std::string filePath
	) : resource(resource), cpuHandle(cpuHandle), gpuHandle(gpuHandle), filePath(filePath) {};
};

class TextureManager
{
public:
	//TextureManagerを取得する
	static TextureManager* GetInstance() {
		static TextureManager instance;
		return &instance;
	}

	static Texture GetEmptyTexture();
	static Texture GetHogeHogeTexture();

	/// <summary>
	/// ファイルからテクスチャを読み込んで登録する
	/// </summary>
	/// <param name="filepath">ファイルへのパス</param>
	/// <returns>読み込んだテクスチャのハンドル</returns>
	static TextureHandle Load(const std::string filepath, const std::string handle = "");

	/// <summary>
	/// 登録済みのテクスチャを取得する
	/// </summary>
	/// <param name="handle">取得するハンドル</param>
	/// <returns></returns>
	static Texture& Get(const TextureHandle& handle);

	/// <summary>
	/// テクスチャを登録する
	/// </summary>
	/// <param name="texture">登録するテクスチャ</param>
	/// <param name="handle">登録先のテクスチャハンドル</param>
	/// <returns>登録先のテクスチャハンドル</returns>
	static TextureHandle Register(Texture texture, TextureHandle handle = "");

	/// <summary>
	/// 登録済みのテクスチャを即座に破棄する
	/// </summary>
	/// <param name="handle">破棄するテクスチャのハンドル</param>
	static void UnRegister(const TextureHandle& handle);

	/// <summary>
	/// 登録済みのテクスチャをそのフレーム終了時に破棄する
	/// そのフレームの描画に使うけどそれ以降いらない場合はこっち
	/// </summary>
	/// <param name="handle">破棄するテクスチャのハンドル</param>
	static void UnRegisterAtEndFrame(const TextureHandle& handle);

	//フレーム終了時の処理を行う(呼べ)
	static void EndFrameProcess();

	//読み込んだテクスチャを全て即座に破棄する
	static void UnRegisterAll();

	ID3D12DescriptorHeap* GetSRVHeap() {
		return this->srvHeap.Get();
	}

private:
	TextureManager() {
		Init();
	};
	~TextureManager() = default;
	TextureManager(const TextureManager& a) {};
	TextureManager& operator=(const TextureManager&) { return *this; }

	void Init();
	
	TextureHandle LoadInternal(const std::string filepath, const std::string handle = "");
	Texture& GetInternal(const TextureHandle& handle);
	TextureHandle RegisterInternal(Texture texture, TextureHandle handle = "");
	void UnRegisterInternal(const TextureHandle& handle);
	void UnRegisterAtEndFrameInternal(const TextureHandle& handle);
	void EndFrameProcessInternal();

	std::recursive_mutex mutex;
	static const UINT numSRVDescritors = 2048; //デスクリプタヒープの数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap; //テクスチャ用SRVデスクリプタヒープ
	std::map<TextureHandle, Texture> textureMap;
	std::list<TextureHandle> unregisterScheduledList; //UnRegisterAtEndFrame予定リスト
};