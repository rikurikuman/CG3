#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"
#include "Mtllib.h"

using namespace std;

bool ModelData::operator==(const ModelData& o) const
{
    return name == o.name && vertexs == o.vertexs && indices == o.indices && material == o.material;
}

bool ModelData::operator!=(const ModelData& o) const
{
    return !(*this == o);
}


ModelHandle Model::Load(string filepath, string filename, ModelHandle handle)
{
    ModelManager* instance = ModelManager::GetInstance();
    Model model;

    if (filepath[filepath.size() - 1] != '/') {
        filepath += "/";
    }

    model.path = filepath + filename;

    ifstream file;
    file.open((filepath + filename).c_str());
    if (file.fail()) {
        return "PreRegistedModel_Empty";
    }

    if (handle.empty()) {
        handle = "NoNameHandle_" + model.path;
    }

    if (instance->modelMap.find(handle) != instance->modelMap.end()
        && instance->modelMap[handle].path == (filepath + filename)) {
        return handle;
    }

    ModelData loading;

    vector<Material> materialList;
    vector<Vector3> vertPosList;
    vector<Vector2> vertTexcoordList;
    vector<Vector3> vertNormalList;

    int cou = 0;

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key == "#") {
            continue;
        }

        if (key == "o") { //おなまえ
            if (loading != ModelData()) {
                loading.vertexBuff.Init(loading.vertexs);
                loading.indexBuff.Init(loading.indices);
                loading.material.Transfer(loading.materialBuff.constMap);
                model.data.emplace_back(std::make_shared<ModelData>(loading));
            }

            loading = ModelData();
            line_stream >> loading.name;
        }

        if (key == "v") { //頂点座標
            Vector3 pos;
            line_stream >> pos.x;
            line_stream >> pos.y;
            line_stream >> pos.z;

            vertPosList.emplace_back(pos.x, pos.y, pos.z);
        }

        if (key == "vt") { //頂点UV
            Vector2 texcoord;
            line_stream >> texcoord.x;
            line_stream >> texcoord.y;
            vertTexcoordList.emplace_back(texcoord.x, 1.0f - texcoord.y);
        }

        if (key == "vn") { //頂点法線
            Vector3 normal;

            line_stream >> normal.x;
            line_stream >> normal.y;
            line_stream >> normal.z;

            vertNormalList.emplace_back(normal.x, normal.y, normal.z);
        }

        if (key == "f") { //インデックス
            vector<string> indexs = Util::StringSplit(line_stream.str(), " ");

            vector<VertexPNU> _vertices;
            for (int i = 1; i < indexs.size(); i++) {
                vector<string> indexText = Util::StringSplit(indexs[i], "/");

                bool ok = false;
                VertexPNU vertex;
                if (indexText.size() >= 1) {
                    int index = atoi(indexText[0].c_str()) - 1;
                    if (index >= 0 && vertPosList.size() > index) {
                        vertex.pos = vertPosList[index];
                        ok = true;
                    }
                }
                if (indexText.size() >= 2) {
                    int index = atoi(indexText[1].c_str()) - 1;
                    if (index >= 0 && vertTexcoordList.size() > index) {
                        vertex.uv = vertTexcoordList[index];
                    }
                }
                if (indexText.size() >= 3) {
                    int index = atoi(indexText[2].c_str()) - 1;
                    if (index >= 0 && vertNormalList.size() > index) {
                        vertex.normal = vertNormalList[index];
                    }
                }

                if (ok) {
                    _vertices.emplace_back(vertex);
                } 
            }

            if (_vertices.size() >= 3) {
                for (int i = 0; i < _vertices.size() - 2; i++) {
                    int indexB = 1 + i;
                    int indexC = 2 + i;

                    //めちゃくちゃ重いから一回廃止
                    /*size_t a = Util::IndexOf(model.vertexs, _vertices[0]);
                    size_t b = Util::IndexOf(model.vertexs, _vertices[indexB]);
                    size_t c = Util::IndexOf(model.vertexs, _vertices[indexC]);*/

                    size_t a = -1;
                    size_t b = -1;
                    size_t c = -1;

                    if (a == -1) {
                        loading.vertexs.emplace_back(_vertices[0]);
                        a = loading.vertexs.size() - 1;
                    }
                    if (b == -1) {
                        loading.vertexs.emplace_back(_vertices[indexB]);
                        b = loading.vertexs.size() - 1;
                    }
                    if (c == -1) {
                        loading.vertexs.emplace_back(_vertices[indexC]);
                        c = loading.vertexs.size() - 1;
                    }

                    loading.indices.emplace_back((UINT)a);
                    loading.indices.emplace_back((UINT)b);
                    loading.indices.emplace_back((UINT)c);
                }
            }
        }

        if (key == "mtllib") { //マテリアルテンプレートライブラリ
            string mfilename;
            line_stream >> mfilename;
            Mtllib lib = Mtllib::Load(filepath, mfilename);

            for (Material& mat : lib.materials) {
                materialList.push_back(mat);
            }
        }

        if (key == "usemtl") {
            if (loading.material != Material()) {
                string oldname = loading.name;
                loading.vertexBuff.Init(loading.vertexs);
                loading.indexBuff.Init(loading.indices);
                loading.material.Transfer(loading.materialBuff.constMap);
                model.data.emplace_back(std::make_shared<ModelData>(loading));
                loading = ModelData();
                loading.name = oldname + "_ChangeMaterial";
            }
            string mtlName;
            line_stream >> mtlName;
            for (Material& mtl : materialList) {
                if (mtl.name == mtlName) {
                    loading.material = mtl;
                }
            }
        }
    }

    loading.vertexBuff.Init(loading.vertexs);
    loading.indexBuff.Init(loading.indices);
    loading.material.Transfer(loading.materialBuff.constMap);
    model.data.emplace_back(std::make_shared<ModelData>(loading));

    instance->modelMap[handle] = model;
    return handle;
}

ModelHandle Model::Register(ModelHandle handle, Model model)
{
    ModelManager* instance = ModelManager::GetInstance();

    instance->modelMap[handle] = model;
    return handle;
}

Model* ModelManager::Get(ModelHandle handle)
{
    ModelManager* instance = GetInstance();

    if (instance->modelMap.find(handle) == instance->modelMap.end()) {
        return &instance->modelMap["PreRegisteredModel_Empty"];
    }

    return &instance->modelMap[handle];
}

void ModelManager::Init()
{
    Model model;
    ModelData data;

    data.vertexs.push_back(VertexPNU({ -1, -1,  1 }, { 0, 0, 0 }, { 0, 1 })); //0
    data.vertexs.push_back(VertexPNU({  1, -1,  1 }, { 0, 0, 0 }, { 1, 1 })); //1
    data.vertexs.push_back(VertexPNU({ -1, -1, -1 }, { 0, 0, 0 }, { 0, 1 })); //2
    data.vertexs.push_back(VertexPNU({  1, -1, -1 }, { 0, 0, 0 }, { 1, 1 })); //3
    data.vertexs.push_back(VertexPNU({  0,  1,  0 }, { 0, 0, 0 }, { 0.5f, 0 })); //4

    data.indices.push_back(2);
    data.indices.push_back(3);
    data.indices.push_back(1);
    data.indices.push_back(2);
    data.indices.push_back(1);
    data.indices.push_back(0);
    data.indices.push_back(2);
    data.indices.push_back(4);
    data.indices.push_back(3);
    data.indices.push_back(3);
    data.indices.push_back(4);
    data.indices.push_back(1);
    data.indices.push_back(1);
    data.indices.push_back(4);
    data.indices.push_back(0);
    data.indices.push_back(0);
    data.indices.push_back(4);
    data.indices.push_back(2);

    VertexPNU::CalcNormalVec(data.vertexs, data.indices);

    data.vertexBuff.Init(data.vertexs);
    data.indexBuff.Init(data.indices);
    data.material.Transfer(data.materialBuff.constMap);
    model.data.emplace_back(std::make_shared<ModelData>(data));
    modelMap["PreRegisteredModel_Empty"] = model;
}
