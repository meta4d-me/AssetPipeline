#pragma once

#include "Constants.h"

namespace cdtools
{

/// <summary>
/// CRTP base class to write common methods about T for all derived vector classes to avoid duplicated codes.
/// </summary>
/// <typeparam name="T"> The numeric type : bool, int, float, double, ... </typeparam>
/// <typeparam name="CRTP"> The CRTP trick which allows you to access members in the derived class. </typeparam>
template<typename T, typename Derived>
class VectorBase
{
private:
	// The CRTP trick.
	Derived& CRTP() { return static_cast<Derived&>(*this); }
	const Derived& CRTP() const { return static_cast<const Derived&>(*this); }

public:
	// Static methods.
	static constexpr Derived Zero()
	{
		Derived derived;
		std::fill(std::begin(derived.data), std::end(derived.data), static_cast<T>(0));
		return derived;
	}

	static constexpr Derived One()
	{
		Derived derived;
		std::fill(std::begin(derived.data), std::end(derived.data), static_cast<T>(1));
		return derived;
	}

public:
	VectorBase() = default;
	VectorBase(const VectorBase&) = default;
	VectorBase& operator=(const VectorBase&) = default;
	VectorBase(VectorBase&&) = default;
	VectorBase& operator=(VectorBase&&) = default;

	// STL style's iterators.
	using iterator = T*;
	using const_iterator = const T*;
	iterator begin() { return &CRTP().data[0]; }
	iterator end() { return &CRTP().data[0] + size(); }
	const_iterator begin() const { return &CRTP().data[0]; }
	const_iterator end() const { return &CRTP().data[0] + size(); }
	constexpr std::size_t size() const { return std::extent<decltype(Derived::data)>::value; }

	// operators
	T& operator[](int index) { return CRTP().data[index]; }
	const T& operator[](int index) const { return CRTP().data[index]; }

	bool operator==(const VectorBase& other)
	{
		for (std::size_t index = 0; index < size(); ++index)
		{
			if constexpr (std::is_integral(T))
			{
				if (CRTP().data[index] != other[index])
				{
					return false;
				}
			}
			else
			{
				if (std::abs(CRTP().data[index] - other[index]) > SmallNumberTolerance)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool operator!=(const VectorBase& other)
	{
		return !this == other;
	}
};

}