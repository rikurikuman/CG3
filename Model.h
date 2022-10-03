#pragma once
#include <string>
#include <vector>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RConstBuffer.h"
#include "Material.h"

typedef std::string ModelHandle;

class ModelData
{
public:
	std::string name;
	VertexBuffer vertexBuff;
	IndexBuffer indexBuff;
	RConstBuffer<MaterialBuffer> materialBuff;

	std::vector<VertexPNU> vertexs;
	std::vector<UINT> indices;
	Material material;

	void UpdateMaterial() {
		material.Transfer(materialBuff.constMap);
	}

	bool operator==(const ModelData& o) const;
	bool operator!=(const ModelData& o) const;
};

class Model
{
public:
	std::string name;
	std::string path;
	std::vector<std::shared_ptr<ModelData>> data;

	static ModelHandle Load(std::string filepath, std::string filename, ModelHandle handle = "");
	static ModelHandle Register(ModelHandle handle, Model model);
};

class ModelManager
{
public:
	ModelManager() {
		Init();
	}

	static ModelManager* GetInstance() {
		static ModelManager manager;
		return &manager;
	}

	static Model* Get(ModelHandle handle);

	std::map<ModelHandle, Model> modelMap;

private:
	void Init();
};