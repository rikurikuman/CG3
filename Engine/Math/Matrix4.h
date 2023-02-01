#pragma once
#include "Vector3.h"
#include <DirectXMath.h>

class Matrix4
{
private:
	class Line {
		float m[4] = {};

	public:
		float operator[](const size_t i) const;
		float& operator[](const size_t i);
	};
	Line m[4] = {};

public:
	//�P�ʍs��
	Matrix4();

	//�O�̂���XMMARTIX����̕ϊ�
	Matrix4(DirectX::XMMATRIX matrix);

	//�Y����
	Line operator[](const size_t i) const;
	Line& operator[](const size_t i);

	//�t�s��
	Matrix4 operator-() const;

	//��r
	bool operator==(const Matrix4& a) const;
	bool operator!=(const Matrix4& a) const;

	//���Z
	Matrix4 operator+(const Matrix4& a) const;
	//���Z
	Matrix4 operator-(const Matrix4& a) const;
	//��Z
	Matrix4 operator*(const Matrix4& a) const;

	//���Z
	Matrix4& operator+=(const Matrix4& a);
	//���Z
	Matrix4& operator-=(const Matrix4& a);
	//��Z
	Matrix4& operator*=(const Matrix4& a);

	//�]�u�s��(���̃C���X�^���X������������
	Matrix4& Transpose();

	//�]�u�s����擾
	//�i���̃C���X�^���X�͏����������ɕʂ̃C���X�^���X�Ƃ��Ď擾����j
	Matrix4 GetTranspose() const;

	//���s�ړ��s��炭�炭����
	static Matrix4 Translation(float x, float y, float z);

	//�X�P�[�����O�s��炭�炭����
	static Matrix4 Scaling(float x, float y, float z);

	//X����]�s��炭�炭����
	static Matrix4 RotationX(float radian);
	//Y����]�s��炭�炭����
	static Matrix4 RotationY(float radian);
	//Z����]�s��炭�炭����
	static Matrix4 RotationZ(float radian);

	//ZXY(Roll,Pitch,Yaw)��]�s��炭�炭����
	static Matrix4 RotationZXY(float radianX, float radianY, float radianZ);

	//�r���[�ϊ��s��y�X����
	static Matrix4 View(Vector3 eye, Vector3 target, Vector3 up);

	//���s���e�ϊ��s��炭�炭����
	static Matrix4 OrthoGraphicProjection(float left, float right, float top, float bottom, float nearZ, float farZ);
	//�������e�ϊ��s��炭�炭����
	static Matrix4 PerspectiveProjection(float fov, float aspect, float nearZ, float farZ);
};

Vector3 operator*(const Vector3 vec, const Matrix4 mat);
Vector3& operator*=(Vector3& vec, const Matrix4 mat);

