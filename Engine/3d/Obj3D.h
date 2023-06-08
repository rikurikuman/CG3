#pragma once
#include "Transform.h"
#include "ViewProjection.h"

class Obj3D
{
protected:
	Obj3D* mParent = nullptr;

public:
	virtual ~Obj3D() {}

	Transform transform;

	void SetParent(Obj3D* parent) {
		mParent = parent;
		this->transform.parent = &parent->transform;
	}

	const Obj3D* GetParent() {
		return mParent;
	}

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	virtual void TransferBuffer(ViewProjection viewprojection) {}

	//�`��v����Renderer��
	virtual void Draw() {}

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	virtual void DrawCommands() {}
};

