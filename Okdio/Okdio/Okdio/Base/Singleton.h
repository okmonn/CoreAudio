#pragma once

namespace okmonn
{
	template <typename T>
	class Singleton
	{
	public:
		// �C���X�^���X�ϐ��擾
		static T& Get(void)
		{
			static T instance;
			return instance;
		}

	private:
		Singleton(const Singleton&) = delete;
		void operator=(const Singleton&) = delete;

	protected:
		// �R���X�g���N�^
		Singleton() {}
		// �f�X�g���N�^
		~Singleton() {}
	};
}