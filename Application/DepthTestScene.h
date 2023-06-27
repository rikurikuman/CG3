#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"
#include <Bloom.h>
#include <CrossFilter.h>
#include <DepthTest.h>

class DepthTestScene : public IScene
{
public:
	DepthTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	struct TestObj {
		ModelObj obj;

		TestObj() {}
		TestObj(ModelHandle model, Vector3 pos, Vector3 rot, Vector3 sca, Color col = {1, 1, 1, 1}) {
			obj = ModelObj(model);
			obj.mTransform.position = pos;
			obj.mTransform.rotation = rot;
			obj.mTransform.scale = sca;
			obj.mTransform.UpdateMatrix();
			obj.mTuneMaterial.mColor = col;
		}
	};

	std::vector<TestObj> objList;

	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	bool useDepthTest = false;
	DepthTest depthTest;;
};