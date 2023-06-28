#pragma once
#include "JsonWrapper.h"
#include <Vector3.h>

namespace LevelLoader {
	struct Transform {
		Vector3 translation;
		Vector3 rotation;
		Vector3 scaling;
	};

	struct Collider {
		std::string type;
		Vector3 center;
		Vector3 size;
	};

	struct LevelObject {
		std::vector<LevelObject> children;
		std::string type;
		std::string name;

		bool hasFileName = false;
		std::string fileName;
		bool hasTransform = false;
		Transform transform;
		bool hasCollider = false;
		Collider collider;
	};

	struct LevelData {
		std::string filepath;
		std::vector<LevelObject> objects;
	};

	LevelData Load(std::string filepath);

	//jsonägí£
	void to_json(json& j, const Transform& o);
	void from_json(const json& j, Transform& o);

	void to_json(json& j, const Collider& o);
	void from_json(const json& j, Collider& o);

	void to_json(json& j, const LevelObject& o);
	void from_json(const json& j, LevelObject& o);
}