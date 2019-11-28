#pragma once

template <typename T>
class Vector4
{
public:
	T x;
	T y;
	T z;
	T w;

	// �R���X�g���N�^
	Vector4() {
		x = y = z = w = T(0);
	}
	Vector4(const T& x, const T& y, const T& z, const T& w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	Vector4(const Vector4& vec) {
		(*this) = vec;
	}
	// �f�X�g���N�^
	~Vector4() {
	}

	// ���Z�q�I�[�o�[���[�h
	void operator=(const Vector4& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
	}
};

typedef Vector4<int>Vec4;
typedef Vector4<float>Vec4f;
