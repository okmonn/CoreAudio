#pragma once

template <typename T>
class Vector3
{
public:
	T x;
	T y;
	T z;

	// �R���X�g���N�^
	Vector3() {
		x = y = z = T(0);
	}
	Vector3(const T& x, const T& y, const T& z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3(const Vector3& vec) {
		(*this) = vec;
	}
	// �f�X�g���N�^
	~Vector3() {
	}

	// ���Z�q�I�[�o�[���[�h
	void operator=(const Vector3& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}
};

typedef Vector3<int> Vec3;
typedef Vector3<float> Vec3f;
