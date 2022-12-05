#pragma once

#include "Constants.h"

#include <algorithm>

namespace cd
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
	VectorBase() = default;
	VectorBase(const VectorBase&) = default;
	VectorBase& operator=(const VectorBase&) = default;
	VectorBase(VectorBase&&) = default;
	VectorBase& operator=(VectorBase&&) = default;
	~VectorBase() = default;

	// STL style's iterators.
	using iterator = T*;
	using const_iterator = const T*;
	iterator begin() { return &CRTP().data[0]; }
	iterator end() { return &CRTP().data[0] + size(); }
	const_iterator begin() const { return &CRTP().data[0]; }
	const_iterator end() const { return &CRTP().data[0] + size(); }
	constexpr std::size_t size() const { return std::extent<decltype(Derived::data)>::value; }

	// Mathematics
	// Single value method is used to operate all components with the same value.
	// TODO : check bad cases, such as infinity(or think it as a good usage), nan(absolutely bad)...
	// TODO : Avoid duplicated codes in a good way?
	Derived& Add(T value)
	{
		std::for_each(this->begin(), this->end(), [&value](T& component) { component += value; });
		return CRTP();
	}
	Derived& Add(const Derived& other)
	{
		int count = 0;
		std::for_each(this->begin(), this->end(), [&other, &count](T& component) { component += other[count++]; });
		return CRTP();
	}

	Derived& Minus(T value)
	{
		std::for_each(this->begin(), this->end(), [&value](T& component) { component -= value; });
		return CRTP();
	}
	Derived& Minus(const Derived& other)
	{
		int count = 0;
		std::for_each(this->begin(), this->end(), [&other, &count](T& component) { component -= other[count++]; });
		return CRTP();
	}

	Derived& Multiply(T value)
	{
		std::for_each(this->begin(), this->end(), [&value](T& component) { component *= value; });
		return CRTP();
	}
	Derived& Multiply(const Derived& other)
	{
		int count = 0;
		std::for_each(this->begin(), this->end(), [&other, &count](T& component) { component *= other[count++]; });
		return CRTP();
	}

	Derived& Divide(T value)
	{
		std::for_each(this->begin(), this->end(), [&value](T& component) { component /= value; });
		return CRTP();
	}
	Derived& Divide(const Derived& other)
	{
		int count = 0;
		std::for_each(this->begin(), this->end(), [&other, &count](T& component) { component /= other[count++]; });
		return CRTP();
	}

	T Length() const { return std::sqrt(LengthSquare()); }
	T LengthSquare() const
	{
		T result {};
		std::for_each(this->begin(), this->end(), [&result](const T& component) { result += component * component; });
		return result;
	}
	
	Derived& Normalize()
	{
		T length = Length();
		std::for_each(this->begin(), this->end(), [&length](T& component) { component /= length; });
		return CRTP();
	}

	T Dot(const Derived& other) 
	{
		T result {};
		int count = 0;
		for (uint32_t i = 0; i < size(); ++i) {
			result += (*this)[i] * other[i];
		}
		return result;
	}

	// Operators
	constexpr T& operator[](int index) { return CRTP().data[index]; }
	constexpr const T& operator[](int index) const { return CRTP().data[index]; }

	bool operator!=(const Derived& other) const { return !this == other; }
	bool operator==(const Derived& other) const
	{
		for (int index = 0; index < size(); ++index)
		{
			if constexpr (std::is_floating_point<T>())
			{
				if (std::abs(CRTP().data[index] - other[index]) > SmallNumberTolerance)
				{
					return false;
				}
			}
			else
			{
				if (CRTP().data[index] != other[index])
				{
					return false;
				}
			}
		}

		return true;
	}
};

}