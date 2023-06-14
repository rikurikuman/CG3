#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RConstBuffer.h"
#include "Material.h"

//assimp
#pragma warning(push)
#pragma warning(disable: 26451)
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#pragma warning(pop)

typedef std::string ModelHandle;

class ModelData
{
public:
	std::string mName;
	VertexBuffer mVertBuff;
	IndexBuffer mIndexBuff;

	std::vector<VertexPNU> mVertices;
	std::vector<uint32_t> mIndices;
	Material mMaterial;

	void CalcSmoothedNormals();

	bool operator==(const ModelData& o) const;
	bool operator!=(const ModelData& o) const;
};

class Model
{
public:
	std::string mName;
	std::string mPath;
	std::vector<std::shared_ptr<ModelData>> mData;

	static ModelHandle Load(std::string filepath, std::string filename, ModelHandle handle = "", bool smooth = false);
	static ModelHandle Register(ModelHandle handle, Model model);

	static ModelHandle LoadWithAIL(std::string directoryPath, std::string filename, ModelHandle handle = "");
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

	std::map<ModelHandle, Model> mModelMap;
	std::recursive_mutex mMutex;

private:
	void Init();
};