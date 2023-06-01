#include "JsonWrapper.h"

void to_json(json& j, const Vector3& o) {
	j = { o.x, o.y, o.z };
}

void from_json(const json& j, Vector3& o) {
	o.x = j[0];
	o.y = j[1];
	o.z = j[2];
}