#pragma once

#include "Math/Matrix.hpp"

namespace cd
{

template<typename T>
class TQuaternion
{
public:
	using ValueType = T;
	static constexpr std::size_t Size = 4;
	using Iterator = T*;
	using ConstIterator = const T*;

	static TQuaternion<T> Identity()
	{
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);
		return TQuaternion<T>(one, zero, zero, zero);
	}

	static TQuaternion<T> FromAxisAngle(const TVector<T, 3>& axis, T angleRadian)
	{
		T halfAngle = angleRadian * static_cast<T>(0.5);
		T sinHalfAngle = std::sin(halfAngle);
		return TQuaternion<T>(std::cos(halfAngle), axis.x() * sinHalfAngle, axis.y() * sinHalfAngle, axis.z() * sinHalfAngle);
	}

	static TQuaternion<T> FromPitchYawRoll(T pitch, T yaw, T roll)
	{
		constexpr T Round = static_cast<T>(360);
		const T rollNoWinding = std::fmod(roll, Round);

		const T pitchNoWinding = std::fmod(pitch, Round);
		const T yawNoWinding = std::fmod(yaw, Round);

		T sinRoll = std::sin(Math::DegreeToRadian<T>(rollNoWinding) * 0.5f);
		T cosRoll = std::cos(Math::DegreeToRadian<T>(rollNoWinding) * 0.5f);
		T sinPitch = std::sin(Math::DegreeToRadian<T>(pitchNoWinding) * 0.5f);
		T cosPitch = std::cos(Math::DegreeToRadian<T>(pitchNoWinding) * 0.5f);
		T sinYaw = std::sin(Math::DegreeToRadian<T>(yawNoWinding) * 0.5f);
		T cosYaw = std::cos(Math::DegreeToRadian<T>(yawNoWinding) * 0.5f);

		return TQuaternion<T>(
			cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw,
			cosRoll * sinPitch * sinYaw - sinRoll * cosPitch * cosYaw,
			-cosRoll * sinPitch * cosYaw - sinRoll * cosPitch * sinYaw,
			cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw);
	}

	static TQuaternion<T> Lerp(const TQuaternion<T>& a, const TQuaternion<T>& b, T t)
	{
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);
		return b * t + a * Math::FloatSelect(a.Dot(b), one, -one) * (one - t);
	}

	static TQuaternion<T> LerpNormalized(const TQuaternion<T>& a, const TQuaternion<T>& b, T t)
	{
		return Lerp(a, b, t).Normalize();
	}

	static TQuaternion<T> BiLerp(const TQuaternion<T>& x0y0, const TQuaternion<T>& x1y0, const TQuaternion<T>& x0y1, const TQuaternion<T>& x1y1, T x, T y)
	{
		return Lerp(Lerp(x0y0, x1y0, x), Lerp(x0y1, x1y1, x), y);
	}

	static TQuaternion<T> BiLerpNormalized(const TQuaternion<T>& x0y0, const TQuaternion<T>& x1y0, const TQuaternion<T>& x0y1, const TQuaternion<T>& x1y1, T x, T y)
	{
		return BiLerp(x0y0, x1y0, x0y1, x1y1, x, y).Normalize();
	}

	// https://tiborstanko.sk/lerp-vs-slerp.html
	static TQuaternion<T> SLerp(const TQuaternion<T>& a, const TQuaternion<T>& b, T t)
	{
		constexpr T one = static_cast<T>(1);

		T rawCosAngle = a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();
		T cosAngle = Math::FloatSelect(rawCosAngle, rawCosAngle, -rawCosAngle);

		T scale0;
		T scale1;

		if (Math::IsSmallThanOne(cosAngle))
		{
			T omega = std::acos(cosAngle);
			T inverseSin = one / std::sin(omega);
			scale0 = std::sin((one - t) * omega) * inverseSin;
			scale1 = std::sin(t * omega) * inverseSin;
		}
		else
		{
			// Linear interpolation.
			scale0 = one - t;
			scale1 = t;
		}

		scale1 = Math::FloatSelect(rawCosAngle, scale1, -scale1);
		return TQuaternion<T>(
			scale0 * a.w() + scale1 * b.w(),
			scale0 * a.x() + scale1 * b.x(),
			scale0 * a.y() + scale1 * b.y(),
			scale0 * a.z() + scale1 * b.z());
	}

	static TQuaternion<T> SLerpNormalized(const TQuaternion<T>& a, const TQuaternion<T>& b, T t)
	{
		return SLerp(a, b, t).Normalize();
	}

	// "Quaternion Calculus and Fast Animation" Ken Shoemake, 1987 SIGGRAPH.
	// intel report https://www.intel.com/content/dam/develop/external/us/en/documents/293748-142817.pdf.
	static TQuaternion<T> FromMatrix(const TMatrix<T, 3, 3>& m)
	{
		constexpr T half = static_cast<T>(0.5);
		constexpr T one = static_cast<T>(1);

		T fourXSquaredMinus1 = m.Data(0, 0) - m.Data(1, 1) - m.Data(2, 2);
		T fourYSquaredMinus1 = m.Data(1, 1) - m.Data(0, 0) - m.Data(2, 2);
		T fourZSquaredMinus1 = m.Data(2, 2) - m.Data(0, 0) - m.Data(1, 1);
		T fourWSquaredMinus1 = m.Data(0, 0) + m.Data(1, 1) + m.Data(2, 2);

		int biggestIndex = 0;
		T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		T biggestVal = std::sqrt(fourBiggestSquaredMinus1 + one) * half;
		T mult = static_cast<T>(0.25) / biggestVal;

		switch (biggestIndex)
		{
		case 0:
			return TQuaternion<T>(biggestVal, (m.Data(1, 2) - m.Data(2, 1)) * mult, (m.Data(2, 0) - m.Data(0, 2)) * mult, (m.Data(0, 1) - m.Data(1, 0)) * mult);
		case 1:
			return TQuaternion<T>((m.Data(1, 2) - m.Data(2, 1)) * mult, biggestVal, (m.Data(0, 1) + m.Data(1, 0)) * mult, (m.Data(2, 0) + m.Data(0, 2)) * mult);
		case 2:
			return TQuaternion<T>((m.Data(2, 0) - m.Data(0, 2)) * mult, (m.Data(0, 1) + m.Data(1, 0)) * mult, biggestVal, (m.Data(1, 2) + m.Data(2, 1)) * mult);
		case 3:
			return TQuaternion<T>((m.Data(0, 1) - m.Data(1, 0)) * mult, (m.Data(2, 0) + m.Data(0, 2)) * mult, (m.Data(1, 2) + m.Data(2, 1)) * mult, biggestVal);
		default:
			return TQuaternion<T>::Identity();
		}
	}

public:
	TQuaternion() = default;
	explicit TQuaternion(T s, T vx, T vy, T vz) : m_scalar(s), m_vector(vx, vy, vz) {}
	explicit TQuaternion(T s, TVector<T, 3> v) : m_scalar(s), m_vector(cd::MoveTemp(v)) {}
	TQuaternion(const TQuaternion&) = default;
	TQuaternion& operator=(const TQuaternion&) = default;
	TQuaternion(TQuaternion&&) = default;
	TQuaternion& operator=(TQuaternion&&) = default;
	~TQuaternion() = default;

	void Clear() { m_scalar = static_cast<T>(1); m_vector.Clear(); }

	// Get
	CD_FORCEINLINE Iterator Begin() { return m_vector.Begin(); }
	CD_FORCEINLINE Iterator End() { return &m_scalar + 1; }
	CD_FORCEINLINE ConstIterator Begin() const { return m_vector.Begin(); }
	CD_FORCEINLINE ConstIterator End() const { return &m_scalar + 1; }
	CD_FORCEINLINE T& Data(int index) { return *(Begin() + index); }
	CD_FORCEINLINE T Data(int index) const { return *(Begin() + index); }
	CD_FORCEINLINE T GetScalar() const { return m_scalar; }
	CD_FORCEINLINE void SetScalar(T s) { m_scalar = s; }
	CD_FORCEINLINE const TVector<T, 3>& GetVector() const { return m_vector; }
	CD_FORCEINLINE void SetVector(TVector<T, 3> v) { m_vector = cd::MoveTemp(v); }
	CD_FORCEINLINE T x() const { return m_vector.x(); }
	CD_FORCEINLINE T y() const { return m_vector.y(); }
	CD_FORCEINLINE T z() const { return m_vector.z(); }
	CD_FORCEINLINE T w() const { return m_scalar; }
	CD_FORCEINLINE T& x() { return m_vector.x(); }
	CD_FORCEINLINE T& y() { return m_vector.y(); }
	CD_FORCEINLINE T& z() { return m_vector.z(); }
	CD_FORCEINLINE T& w() { return m_scalar; }

	// Validations
	CD_FORCEINLINE bool IsNaN() const { return std::isnan(m_scalar) || std::isnan(m_vector.x()) || std::isnan(m_vector.y()) || std::isnan(m_vector.z()); }

	// Conversions
	TMatrix<T, 3, 3> ToMatrix3x3() const
	{
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T tx = two * x();
		T ty = two * y();
		T tz = two * z();
		T twx = tx * w();
		T twy = ty * w();
		T twz = tz * w();
		T txx = tx * x();
		T txy = ty * x();
		T txz = tz * x();
		T tyy = ty * y();
		T tyz = tz * y();
		T tzz = tz * z();

		return TMatrix<T, 3, 3>(one - (tyy + tzz), txy - twz, txz + twy,
			txy + twz, one - (txx + tzz), tyz - twx,
			txz - twy, tyz + twx, one - (txx + tyy));
	}

	TMatrix<T, 4, 4> ToMatrix4x4() const
	{
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T tx = two * x();
		T ty = two * y();
		T tz = two * z();
		T twx = tx * w();
		T twy = ty * w();
		T twz = tz * w();
		T txx = tx * x();
		T txy = ty * x();
		T txz = tz * x();
		T tyy = ty * y();
		T tyz = tz * y();
		T tzz = tz * z();

		// RHS
		//return TMatrix<T, 4, 4>(one - (tyy + tzz), txy + twz, txz - twy, zero,
		//	txy - twz, one - (txx + tzz), tyz + twx, zero,
		//	txz + twy, tyz - twx, one - (txx + tyy), zero,
		//	zero, zero, zero, one);
		 
		// LHS
		return TMatrix<T, 4, 4>(one - (tyy + tzz), txy - twz, txz + twy, zero,
			txy + twz, one - (txx + tzz), tyz - twx, zero,
			txz - twy, tyz + twx, one - (txx + tyy), zero,
			zero, zero, zero, one);
	}

	CD_FORCEINLINE T Roll() const
	{
		constexpr T two = static_cast<T>(2);

		T resultX = w() * w() - x() * x() - y() * y() + z() * z();
		T resultY = two * (x() * y() + w() * z());

		if (Math::IsEqualToZero(resultX) && Math::IsEqualToZero(resultY))
		{
			// avoid atan2(0, 0)
			return static_cast<T>(two * std::atan2(x(), w()));
		}

		return static_cast<T>(std::atan2(resultY, resultX));
	}

	CD_FORCEINLINE T Pitch() const
	{
		constexpr T two = static_cast<T>(2);

		T resultX = w() * w() - x() * x() - y() * y() + z() * z();
		T resultY = two * (y() * z() + w() * x());

		if (Math::IsEqualToZero(resultX) && Math::IsEqualToZero(resultY))
		{
			// avoid atan2(0, 0)
			return static_cast<T>(two * std::atan2(x(), w()));
		}

		return static_cast<T>(std::atan2(resultY, resultX));
	}

	CD_FORCEINLINE T Yaw() const
	{
		return std::asin(std::clamp(static_cast<T>(-2) * (x() * z() - w() * y()), static_cast<T>(-1), static_cast<T>(1)));
	}

	CD_FORCEINLINE TVector<T, 3> ToEulerAngles()
	{
		return TVector<T, 3>(Math::RadianToDegree(Pitch()), Math::RadianToDegree(Yaw()), Math::RadianToDegree(Roll()));
	}

	// Calculations
	CD_FORCEINLINE TQuaternion<T> Inverse() const { return TQuaternion<T>(m_scalar, -m_vector); }
	CD_FORCEINLINE T Dot(const TQuaternion& rhs) const { return m_scalar * rhs.m_scalar + m_vector.x() * rhs.m_vector.x() + m_vector.y() * rhs.m_vector.y() + m_vector.z() * rhs.m_vector.z(); }
	CD_FORCEINLINE T LengthSqure() const { return Dot(*this);  }
	CD_FORCEINLINE T Length() const { return std::sqrt(LengthSqure());  }
	TQuaternion<T>& Normalize()
	{
		T factor = static_cast<T>(1) / Length();
		m_scalar *= factor;
		m_vector *= factor;
		return *this;
	}

	// Operators
	CD_FORCEINLINE TQuaternion<T> operator+(const TQuaternion<T>& rhs) const { return TQuaternion<T>(m_scalar + rhs.m_scalar, m_vector + rhs.m_vector); }
	CD_FORCEINLINE TQuaternion<T>& operator+=(const TQuaternion<T>& rhs) { m_scalar += rhs.m_scalar; m_vector += rhs.m_vector; return *this; }
	
	CD_FORCEINLINE TQuaternion<T> operator-() const { return TQuaternion<T>(-m_scalar, -m_vector.x(), -m_vector.y(), -m_vector.z()); }
	CD_FORCEINLINE TQuaternion<T> operator-(const TQuaternion<T>& rhs) const { return TQuaternion<T>(m_scalar - rhs.m_scalar, m_vector - rhs.m_vector); }
	CD_FORCEINLINE TQuaternion<T>& operator-=(const TQuaternion<T>& rhs) { m_scalar -= rhs.m_scalar; m_vector -= rhs.m_vector; return *this; }

	CD_FORCEINLINE TQuaternion<T> operator*(T scalar) const { return TQuaternion<T>(m_scalar * scalar, m_vector.x() * scalar, m_vector.y() * scalar, m_vector.z() * scalar); }
	CD_FORCEINLINE TQuaternion<T> operator*(const TQuaternion<T>& rhs) const
	{
		return TQuaternion<T>(m_scalar * rhs.m_scalar - m_vector.Dot(rhs.m_vector),
				rhs.m_vector * m_scalar + m_vector * rhs.m_scalar + m_vector.Cross(rhs.m_vector));
	}
	CD_FORCEINLINE TVector<T, 3> operator*(const TVector<T, 3>& v) const
	{
		T doubleScalar = m_scalar + m_scalar;
		return m_vector.Cross(v) * doubleScalar + v * (doubleScalar * m_scalar - static_cast<T>(1)) + m_vector * m_vector.Dot(v) * static_cast<T>(2);
	}

	CD_FORCEINLINE TQuaternion<T> operator/(T scalar) const { return TQuaternion<T>(m_scalar / scalar, m_vector.x() / scalar, m_vector.y() / scalar, m_vector.z() / scalar); }

	CD_FORCEINLINE bool operator==(const TQuaternion& rhs) const { return m_scalar == rhs.m_scalar && m_vector == rhs.m_vector; }
	CD_FORCEINLINE bool operator!=(const TQuaternion& rhs) const { return !(*this == rhs); }

private:
	TVector<T, 3> m_vector;
	T m_scalar;
};
	
using Quaternion = TQuaternion<float>;

static_assert(4 * sizeof(float) == sizeof(Quaternion));
static_assert(std::is_standard_layout_v<Quaternion> && std::is_trivial_v<Quaternion>);

}