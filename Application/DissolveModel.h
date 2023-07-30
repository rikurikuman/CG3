#pragma once
#include "ModelObj.h"
class DissolveModel : public ModelObj
{
public:
	struct DataDissolve {
		float t;
	};

	DataDissolve dataDissolve{};
	TextureHandle dissolveTex;
	Material mTuneMaterialD = Material(true);

	DissolveModel() {};
	DissolveModel(ModelHandle handle) : ModelObj(ModelManager::Get(handle)) {};

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	void Draw() override;

private:
	static RootSignature& GetRootSignature();
	static GraphicsPipeline& GetGraphicsPipelineA();
	static GraphicsPipeline& GetGraphicsPipelineB();

	std::unordered_map<std::string, SRConstBuffer<MaterialBuffer>> mMaterialBuffDMap;
	SRConstBuffer<DataDissolve> dissolveDataBuff;
};

