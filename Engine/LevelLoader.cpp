#include "LevelLoader.h"
#include <fstream>
#include <iostream>
#include <Util.h>

using namespace LevelLoader;

LevelData LevelLoader::Load(std::string filepath)
{
	std::ifstream file;
	file.open(filepath);
	if (file.fail()) {
		assert(0);
	}

	LevelData data;
	data.filepath = filepath;
	json jsonData;
	try {
		file >> jsonData;
		
		std::string name = jsonData.at("name").get<std::string>();
		assert(name.compare("scene") == 0);

		for (json& object : jsonData.at("objects")) {
			data.objects.emplace_back(object.get<LevelObject>());
		}
	}
	catch (json::exception ex) {
		assert(0);
	}
	return data;
}

void LevelLoader::to_json(json& j, const Transform& o)
{
	j["translation"] = o.translation;
	j["rotation"] = o.rotation;
	j["scaling"] = o.scaling;
}

void LevelLoader::from_json(const json& j, Transform& o)
{
	o.translation = j.at("translation").get<Vector3>();

	//‰ñ“]‚¾‚¯‚¿‚å‚Á‚Æ‚ß‚ñ‚Ç‚¢
	Vector3 rotation = j.at("rotation").get<Vector3>();
	o.rotation.x = Util::AngleToRadian(rotation.x);
	o.rotation.y = Util::AngleToRadian(rotation.y);
	o.rotation.z = Util::AngleToRadian(rotation.z);

	o.scaling = j.at("scaling").get<Vector3>();
}

void LevelLoader::to_json(json& j, const Collider& o)
{
	j["type"] = o.type;
	j["center"] = o.center;
	j["size"] = o.size;
}

void LevelLoader::from_json(const json& j, Collider& o)
{
	o.type = j.at("type").get<std::string>();
	o.center = j.at("center").get<Vector3>();
	o.size = j.at("size").get<Vector3>();
}

void LevelLoader::to_json(json& j, const LevelObject& o)
{
	j["type"] = o.type;
	j["name"] = o.name;
	if(o.hasTransform) j["transform"] = o.transform;
	if(o.hasCollider) j["collider"] = o.collider;
}

void LevelLoader::from_json(const json& j, LevelObject& o)
{
	o.type = j.at("type").get<std::string>();
	o.name = j.at("name").get<std::string>();

	if (j.contains("file_name")) {
		o.hasFileName = true;
		o.fileName = j.at("file_name").get<std::string>();
	}

	if (o.type == "MESH") {
		o.hasTransform = true;
		o.transform = j.at("transform").get<Transform>();

		if (j.contains("collider")) {
			o.hasCollider = true;
			o.collider = j.at("collider").get<Collider>();
		}
	}

	if (j.contains("children")) {
		for (const json& child : j.at("children")) {
			o.children.emplace_back(child.get<LevelObject>());
		}
	}
}
