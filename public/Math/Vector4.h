#pragma once

#include <cmath>

namespace cdtools
{

// Used for float or double.
constexpr double SmallNumberTolerance = 1.e-8f;

enum class Vector4Type
{
	Point,
	Direction,
	Color,
	UV,
};

template<typename Value, Vector4Type Vty>
class Vector4 final
{
public:
	// static methods
	static constexpr Vector4<Value, Vty> Zero()
	{
		return Vector4<Value, Vty>(static_cast<Value>(0), static_cast<Value>(0), static_cast<Value>(0), static_cast<Value>(0));
	}

	static constexpr Vector4<Value, Vty> One()
	{
		return Vector4<Value, Vty>(static_cast<Value>(1), static_cast<Value>(1), static_cast<Value>(1), static_cast<Value>(1));
	}

	static constexpr Value DefaultValueX() { return static_cast<Value>(0); }

	static constexpr Value DefaultValueY()
	{
		if constexpr (Vector4Type::UV == Vty)
		{
			return static_cast<Value>(1);
		}
		else
		{
			return static_cast<Value>(0);
		}
	}

	static constexpr Value DefaultValueZ() { return static_cast<Value>(0); }

	static constexpr Value DefaultValueW()
	{
		if constexpr (Vector4Type::Direction == Vty)
		{
			return static_cast<Value>(0);
		}
		else
		{
			return static_cast<Value>(1);
		}
	}

public:
	Vector4() { Reset(); }
	explicit Vector4(Value x, Value y, Value z, Value w) { Set(x, y, z, w); }
	explicit Vector4(Value x, Value y, Value z) { Set(x, y, z); }
	Vector4(const Vector4&) = default;
	Vector4& operator=(const Vector4&) = default;
	Vector4(Vector4&&) = default;
	Vector4& operator=(Vector4&&) = default;

	// Get/Set methods
	Value& x() { return m_data[0]; }
	const Value& x() const { return m_data[0]; }
	Value& y() { return m_data[1]; }
	const Value& y() const { return m_data[1]; }
	Value& z() { return m_data[2]; }
	const Value& z() const { return m_data[2]; }
	Value& w() { return m_data[3]; }
	const Value& w() const { return m_data[3]; }

	void Reset()
	{
		Set();
	}

	void Set(Value inX = DefaultValueX(), Value inY = DefaultValueY(), Value inZ = DefaultValueZ(), Value inW = DefaultValueW())
	{
		x() = inX;
		y() = inY;
		z() = inZ;
		w() = inW;
	}

	// operators
	Value& operator[](int index) { return m_data[index]; }
	const Value& operator[](int index) const { return m_data[index]; } 

	bool operator==(const Vector4& other)
	{
		if constexpr(std::is_integral(Value))
		{
			return x() == other.x() && y() == other.y() && z() == other.z() && w() == other.w();
		}
		else
		{
			return std::abs(x() - other.x()) <= SmallNumberTolerance &&
				std::abs(y() - other.y()) <= SmallNumberTolerance &&
				std::abs(z() - other.z()) <= SmallNumberTolerance &&
				std::abs(w() - other.w()) <= SmallNumberTolerance;
		}
	}

	bool operator!=(const Vector4& other)
	{
		return !this == other;
	}

private:
	Value m_data[4];
};

using Point = Vector4<double, Vector4Type::Point>;
using Direction = Vector4<double, Vector4Type::Direction>;
using Color = Vector4<double, Vector4Type::Color>;
using U8Color = Vector4<uint8_t, Vector4Type::Color>;

// TJJ TODO : Add a Vector2 class but no repeat!
// Here is the workaround. As we can wrap uv and lightmap's uv into a vector4.
using UV = Vector4<double, Vector4Type::UV>;

}