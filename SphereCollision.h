#pragma once
#include "Collision.h"
#include "Vector3.h"

class Raycast;

class SphereCollision : public Collision
{
public:
	Vector3 pos;
	float r;

	SphereCollision() : pos({ 0, 0, 0 }), r(1) {}
	SphereCollision(Vector3 pos, float r) : pos(pos), r(r) {}

	/// <summary>
	/// ��Collision�Ƃ̓����蔻������
	/// ��Collision�̌^�͖��Ȃ����A
	/// ���̌^�Ƃ̓����蔻�肪����`�Ȃ��O���X���[�����
	/// </summary>
	/// <param name="col">�����蔻����s�������Collision�̎Q��</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	virtual bool Collide(Collision& col) const override;

	/// <summary>
	/// ���Ƃ̓����蔻������
	/// </summary>
	/// <param name="col">�����蔻����s�������SphereCollision</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	bool Collide(const SphereCollision& col) const;

	/// <summary>
	/// Ray�Ƃ̓����蔻������
	/// </summary>
	/// <param name="col">�����蔻����s�������Ray</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	bool Collide(const Raycast& col) const;
};

