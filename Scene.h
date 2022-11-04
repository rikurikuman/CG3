#pragma once
class Scene
{
public:
	virtual ~Scene() {}

	virtual void Init() {}
	virtual void Update() {}
	virtual void Draw() {}
};

