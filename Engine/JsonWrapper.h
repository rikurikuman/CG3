#pragma once
#include <json.hpp>
#include <Vector3.h>

using json = nlohmann::json;

void to_json(json& j, const Vector3& o);
void from_json(const json& j, Vector3& o);