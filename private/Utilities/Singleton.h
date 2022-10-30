#pragma once

#include <memory>

// Just throw this in the CPP file that uses the singleton class
#define DECLARE_SINGLETON_INSTANCE_WITH_TYPE(type) template<>		\
	std::unique_ptr<type> qe::Singleton<type>::mInstance = nullptr;

namespace cdtools
{

	template<typename T >
	class Singleton
	{

	public:

		static T& getInstance()
		{
			QEASSERT(exists());
			return *mInstance;
		}

		template<typename...Args>
		static T& createInstance(Args&&...args)
		{
			QEASSERT(!mInstance);
			mInstance = std::make_unique<T>(std::forward<Args>(args)...);
			return *mInstance;
		}

		static bool exists()
		{
			return mInstance != nullptr;
		}

		static void destroyInstance()
		{
			mInstance = nullptr;
		}

	protected:

		Singleton()
		{
			QEASSERT(exists() == false);
		}

		~Singleton()
		{}

		static std::unique_ptr<T> mInstance;
	};

}	// cdtools