#include "LightGroup.h"

LightGroup* LightGroup::nowLight = nullptr;

void LightGroup::SetDefault()
{
	change = true;

	ambientColor = { 1, 1, 1 };
	
	directionalLights[0].active = true;
	directionalLights[0].lightVec = { 1, -1, 1 };
	directionalLights[0].color = { 1, 1, 1, 1 };

	for (int32_t i = 1; i < directionalLightNum; i++) {
		directionalLights[i].active = false;
	}

	for (int32_t i = 0; i < pointLightNum; i++) {
		pointLights[i].active = false;
	}

	for (int32_t i = 0; i < spotLightNum; i++) {
		spotLights[i].active = false;
	}
}

void LightGroup::Update()
{
	if (change) {
		change = false;

		buffer.constMap->ambientColor = ambientColor;

		for (int32_t i = 0; i < directionalLightNum; i++) {
			if (directionalLights[i].active) {
				directionalLights[i].TransferBuffer(buffer.constMap->directionalLights[i]);
			}
			else {
				buffer.constMap->directionalLights[i].active = false;
			}
		}

		for (int32_t i = 0; i < pointLightNum; i++) {
			if (pointLights[i].active) {
				pointLights[i].TransferBuffer(buffer.constMap->pointLights[i]);
			}
			else {
				buffer.constMap->pointLights[i].active = false;
			}
		}

		for (int32_t i = 0; i < spotLightNum; i++) {
			if (spotLights[i].active) {
				spotLights[i].TransferBuffer(buffer.constMap->spotLights[i]);
			}
			else {
				buffer.constMap->spotLights[i].active = false;
			}
		}
	}
}

void LightGroup::SetAmbientColor(Vector3 color)
{
	ambientColor = color;
}

Vector3 LightGroup::GetAmbientColor()
{
	return ambientColor;
}

void LightGroup::SetDirectionalLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < directionalLightNum);
	directionalLights[index].active = active;
	change = true;
}

bool LightGroup::GetDirectionalLightActive(int32_t index)
{
	assert(0 <= index && index < directionalLightNum);
	return directionalLights[index].active;
}

void LightGroup::SetDirectionalLightVec(int32_t index, Vector3 vec)
{
	assert(0 <= index && index < directionalLightNum);
	directionalLights[index].lightVec = vec;
	change = true;
}

Vector3 LightGroup::GetDirectionalLightVec(int32_t index)
{
	assert(0 <= index && index < directionalLightNum);
	return directionalLights[index].lightVec;
}

void LightGroup::SetDirectionalLightColor(int32_t index, Color color)
{
	assert(0 <= index && index < directionalLightNum);
	directionalLights[index].color = color;
	change = true;
}

Color LightGroup::GetDirectionalLightColor(int32_t index)
{
	assert(0 <= index && index < directionalLightNum);
	return directionalLights[index].color;
}

void LightGroup::SetPointLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < pointLightNum);
	pointLights[index].active = active;
	change = true;
}

bool LightGroup::GetPointLightActive(int32_t index)
{
	assert(0 <= index && index < pointLightNum);
	return pointLights[index].active;
}

void LightGroup::SetPointLightPos(int32_t index, Vector3 pos)
{
	assert(0 <= index && index < pointLightNum);
	pointLights[index].pos = pos;
	change = true;
}

Vector3 LightGroup::GetPointLightPos(int32_t index)
{
	assert(0 <= index && index < pointLightNum);
	return pointLights[index].pos;
}

void LightGroup::SetPointLightColor(int32_t index, Color color)
{
	assert(0 <= index && index < pointLightNum);
	pointLights[index].color = color;
	change = true;
}

Color LightGroup::GetPointLightColor(int32_t index)
{
	assert(0 <= index && index < pointLightNum);
	return pointLights[index].color;
}

void LightGroup::SetPointLightAtten(int32_t index, Vector3 atten)
{
	assert(0 <= index && index < pointLightNum);
	pointLights[index].atten = atten;
	change = true;
}

Vector3 LightGroup::GetPointLightAtten(int32_t index)
{
	assert(0 <= index && index < pointLightNum);
	return pointLights[index].atten;
}

void LightGroup::SetSpotLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < spotLightNum);
	spotLights[index].active = active;
	change = true;
}

bool LightGroup::GetSpotLightActive(int32_t index)
{
	assert(0 <= index && index < spotLightNum);
	return spotLights[index].active;
}

void LightGroup::SetSpotLightPos(int32_t index, Vector3 pos)
{
	assert(0 <= index && index < spotLightNum);
	spotLights[index].pos = pos;
	change = true;
}

Vector3 LightGroup::GetSpotLightPos(int32_t index)
{
	assert(0 <= index && index < spotLightNum);
	return spotLights[index].pos;
}

void LightGroup::SetSpotLightDirection(int32_t index, Vector3 dir)
{
	assert(0 <= index && index < spotLightNum);
	spotLights[index].direction = dir;
	change = true;
}

Vector3 LightGroup::GetSpotLightDirection(int32_t index)
{
	assert(0 <= index && index < spotLightNum);
	return spotLights[index].direction;
}

void LightGroup::SetSpotLightColor(int32_t index, Color color)
{
	assert(0 <= index && index < spotLightNum);
	spotLights[index].color = color;
	change = true;
}

Color LightGroup::GetSpotLightColor(int32_t index)
{
	assert(0 <= index && index < spotLightNum);
	return spotLights[index].color;
}

void LightGroup::SetSpotLightAtten(int32_t index, Vector3 atten)
{
	assert(0 <= index && index < spotLightNum);
	spotLights[index].atten = atten;
	change = true;
}

Vector3 LightGroup::GetSpotLightAtten(int32_t index)
{
	assert(0 <= index && index < spotLightNum);
	return spotLights[index].atten;
}

void LightGroup::SetSpotLightFactorAngle(int32_t index, Vector2 factorAngle)
{
	assert(0 <= index && index < spotLightNum);
	spotLights[index].factorAngle = factorAngle;
	change = true;
}

Vector2 LightGroup::GetSpotLightFactorAngle(int32_t index)
{
	assert(0 <= index && index < spotLightNum);
	return spotLights[index].factorAngle;
}
