#pragma once
class Collision
{
public:
	virtual ~Collision() {};

	/// <summary>
	/// ��Collision�Ƃ̓����蔻������
	/// ��Collision�̌^�͖��Ȃ����A
	/// ���̌^�Ƃ̓����蔻�肪����`�Ȃ��O���X���[�����
	/// </summary>
	/// <param name="col">�����蔻����s�������Collision</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	virtual bool Collide(Collision& col) const = 0;
};

