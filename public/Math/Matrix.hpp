#pragma once

#include "Math/Vector.hpp"

namespace cd
{

enum class MatrixMajor
{
	Column,
	Row
};

// It depends on current choosen graphics API.
// DirectX : ZeroToOne
// OpenGL : MinusOneToOne
enum class NDCDepth
{
	ZeroToOne, // [0, 1]
	MinusOneToOne // [-1, 1]
};

// TMatrix is column-major order.
template<typename T, std::size_t Rows, std::size_t Cols>
class TMatrix
{
public:
	using MatrixType = TMatrix<T, Rows, Cols>;
	static constexpr std::size_t RowCount = Rows;
	static constexpr std::size_t ColCount = Cols;
	static constexpr std::size_t Size = RowCount * ColCount;
	using Iterator = T*;
	using ConstIterator = const T*;

	static MatrixType Identity()
	{
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);

		if constexpr (3 == Rows && 3 == Cols)
		{
			return MatrixType(one, zero, zero, zero, one, zero, zero, zero, one);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return MatrixType(one, zero, zero, zero, zero, one, zero, zero, zero, zero, one, zero, zero, zero, zero, one);
		}
	}

	template<Handedness Hand>
	static MatrixType LookAt(const TVector<T, 3>& eye, const TVector<T, 3>& at, const TVector<T, 3>& up)
	{
		static_assert(4 == Rows && 4 == Cols);
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);

		TVector<T, 3> view;
		if constexpr (Handedness::Left == Hand)
		{
			view = at - eye;
		}
		else
		{
			view = eye - at;
		}
		view.Normalize();

		TVector<T, 3> right = up.Cross(view).Normalize();
		TVector<T, 3> upDirection = view.Cross(right);

		return MatrixType(right.x(), upDirection.x(), view.x(), zero,
                          right.y(), upDirection.y(), view.y(), zero,
                          right.z(), upDirection.z(), view.z(), zero,
                          -right.Dot(eye), -upDirection.Dot(eye), -view.Dot(eye), one);
	}

	template<Handedness Hand, NDCDepth NDC>
	static MatrixType Perspective(T fovy, T aspect, T nearPlane, T farPlane)
	{
		static_assert(4 == Rows && 4 == Cols);
		constexpr T zero = static_cast<T>(0);
		constexpr T half = static_cast<T>(0.5);
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T height = one / std::tan(Math::DegreeToRadian<T>(fovy) * half);
		T width = height * one / aspect;
		T delta = farPlane - nearPlane;

		T aa;
		T bb;
		if constexpr (NDCDepth::MinusOneToOne == NDC)
		{
			aa = (farPlane + nearPlane) / delta;
			bb = (two * farPlane * nearPlane) / delta;
		}
		else
		{
			aa = farPlane / delta;
			bb = nearPlane * aa;
		}

		constexpr T xx = zero;
		constexpr T yy = zero;
		if constexpr (Handedness::Left == Hand)
		{
			return MatrixType(width, zero, zero, zero,
                              zero, height, zero, zero,
                              -xx, -yy, aa, one,
                              zero, zero, -bb, zero);
		}
		else
		{
			return MatrixType(width, zero, zero, zero,
                              zero, height, zero, zero,
                              xx, yy, -aa, -one,
                              zero, zero, -bb, zero);
		}
	}

	static MatrixType Perspective(T fovy, T aspect, T nearPlane, T farPlane, bool isNDCDepthHomogeneous)
	{
		static_assert(4 == Rows && 4 == Cols);
		if (isNDCDepthHomogeneous)
		{
			return MatrixType::Perspective<cd::Handedness::Left, cd::NDCDepth::MinusOneToOne>(fovy, aspect, nearPlane, farPlane);
		}
		else
		{
			return MatrixType::Perspective<cd::Handedness::Left, cd::NDCDepth::ZeroToOne>(fovy, aspect, nearPlane, farPlane);
		}
	}

	template<Handedness Hand, NDCDepth NDC>
	static MatrixType Orthographic(T left, T right, T top, T bottom, T nearPlane, T farPlane, T offset)
	{
		static_assert(4 == Rows && 4 == Cols);
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T deltaX = right - left;
		T deltaY = top - bottom;
		T deltaZ = farPlane - nearPlane;

		T aa = two / deltaX;
		T bb = two / deltaY;
		T dd = (left + right) / -deltaX;
		T ee = (top + bottom) / -deltaY;

		T cc;
		T ff;
		if constexpr (NDCDepth::MinusOneToOne == NDC)
		{
			cc = two / deltaZ;
			ff = (nearPlane + farPlane) / -deltaZ;
		}
		else
		{
			cc = one / deltaZ;
			ff = nearPlane / -deltaZ;
		}

		if constexpr (Handedness::Left == Hand)
		{
			return MatrixType(aa, zero, zero, zero,
                              zero, bb, zero, zero,
                              zero, zero, cc, zero,
                              dd, ee, ff, one);
		}
		else
		{
			return MatrixType(aa, zero, zero, zero,
                              zero, bb, zero, zero,
                              zero, zero, -cc, zero,
                              dd, ee, ff, one);
		}
	}

	static MatrixType Orthographic(T left, T right, T top, T bottom, T nearPlane, T farPlane, T offset, bool isNDCDepthHomogeneous)
	{
		static_assert(4 == Rows && 4 == Cols);
		if (isNDCDepthHomogeneous)
		{
			return MatrixType::Orthographic<cd::Handedness::Left, cd::NDCDepth::MinusOneToOne>(left, right, top, bottom, nearPlane, farPlane, offset);
		}
		else
		{
			return MatrixType::Orthographic<cd::Handedness::Left, cd::NDCDepth::ZeroToOne>(left, right, top, bottom, nearPlane, farPlane, offset);
		}
	}

	/// <summary>
	/// Convert 2D window position to a 3D position in the world space.
	/// </summary>
	/// <param name="window"> The 2D window's selected position. Z is between nearPlane and farPlane in range [0, 1]. </param>
	/// <param name="view"> The model view matrix. </param>
	/// <param name="projection"> The projection matrix. </param>
	/// <param name="viewport"> (x, y, w, h) The 2D window's left top 2D position, width, height. </param>
	/// <returns> 3D position in the world space. </returns>
	static TVector<T, 3> UnProject(const TVector<T, 3>& window, const MatrixType& view, const MatrixType& projection, const cd::Vec4f& viewport)
	{
		static_assert(4 == Rows && 4 == Cols);
		constexpr T one = static_cast<T>(1);

		//assert(window.z() >= static_cast<T>(0) && window.z() <= one);
		MatrixType inversePV = (projection * view).Inverse();
		TVector<T, 4> standard(Math::GetValueInNewRange((window.x() - viewport.x()) / viewport.z(), 0.0f, 1.0f, -1.0f, 1.0f),
			                   Math::GetValueInNewRange((window.y() - viewport.y()) / viewport.w(), 0.0f, 1.0f, -1.0f, 1.0f),
			                   Math::GetValueInNewRange(window.z() / one, 0.0f, 1.0f, -1.0f, 1.0f), one);
		TVector<T, 4> multiply = inversePV * standard;
		TVector<T, 3> result = multiply.xyz();
		result /= multiply.w();
		// assert(multiply.w() != 0);
		return result;
	}

public:
	// Default uninitialized.
	TMatrix() = default;

	// 3x3
	TMatrix(T a00, T a01, T a02,
			T a03, T a04, T a05,
			T a06, T a07, T a08)
	{
		static_assert(3 == Rows && 3 == Cols);
		data[0] = TVector<T, 3>(a00, a01, a02);
		data[1] = TVector<T, 3>(a03, a04, a05);
		data[2] = TVector<T, 3>(a06, a07, a08);
	}

	// 3x3
	TMatrix(TVector<T, 3> colVec0, TVector<T, 3> colVec1, TVector<T, 3> colVec2)
	{
		static_assert(3 == Rows && 3 == Cols);
		data[0] = cd::MoveTemp(colVec0);
		data[1] = cd::MoveTemp(colVec1);
		data[2] = cd::MoveTemp(colVec2);
	}

	// 4x4
	TMatrix(T a00, T a01, T a02, T a03,
			T a04, T a05, T a06, T a07,
			T a08, T a09, T a10, T a11,
			T a12, T a13, T a14, T a15)
	{
		static_assert(4 == Rows && 4 == Cols);
		data[0] = TVector<T, 4>(a00, a01, a02, a03);
		data[1] = TVector<T, 4>(a04, a05, a06, a07);
		data[2] = TVector<T, 4>(a08, a09, a10, a11);
		data[3] = TVector<T, 4>(a12, a13, a14, a15);
	}

	// 4x4
	TMatrix(TVector<T, 4> colVec0, TVector<T, 4> colVec1, TVector<T, 4> colVec2, TVector<T, 4> colVec3)
	{
		static_assert(4 == Rows && 4 == Cols);
		data[0] = cd::MoveTemp(colVec0);
		data[1] = cd::MoveTemp(colVec1);
		data[2] = cd::MoveTemp(colVec2);
		data[3] = cd::MoveTemp(colVec3);
	}

	TMatrix(const TMatrix&) = default;
	TMatrix& operator=(const TMatrix&) = default;
	TMatrix(TMatrix&&) = default;
	TMatrix& operator=(TMatrix&&) = default;
	~TMatrix() = default;

	// Get
	CD_FORCEINLINE Iterator Begin() { return &data[0][0]; }
	CD_FORCEINLINE Iterator End() { return &data[0][0] + Size; }
	CD_FORCEINLINE ConstIterator Begin() const { return &data[0][0]; }
	CD_FORCEINLINE ConstIterator End() const { return &data[0][0] + Size; }
	CD_FORCEINLINE const TVector<T, Rows>& GetColumn(int index) const { return data[index]; }
	CD_FORCEINLINE TVector<T, Rows>& GetColumn(int index) { return data[index]; }
	CD_FORCEINLINE T Data(int index) const { return reinterpret_cast<const T*>(data)[index]; }
	CD_FORCEINLINE T& Data(int index) { return reinterpret_cast<T*>(data)[index]; }
	CD_FORCEINLINE T Data(int row, int col) const { return data[col][row]; }
	CD_FORCEINLINE T& Data(int row, int col) { return data[col][row]; }
	void Clear() { std::memset(Begin(), 0, Size * sizeof(float)); }

	// Calculations
	MatrixType Inverse() const
	{
		static_assert(4 == Rows && 4 == Cols);

		T xx = Data(0);
		T xy = Data(1);
		T xz = Data(2);
		T xw = Data(3);
		T yx = Data(4);
		T yy = Data(5);
		T yz = Data(6);
		T yw = Data(7);
		T zx = Data(8);
		T zy = Data(9);
		T zz = Data(10);
		T zw = Data(11);
		T wx = Data(12);
		T wy = Data(13);
		T wz = Data(14);
		T ww = Data(15);

		T det = static_cast<T>(0);
		det += xx * (yy * (zz * ww - zw * wz) - yz * (zy * ww - zw * wy) + yw * (zy * wz - zz * wy));
		det -= xy * (yx * (zz * ww - zw * wz) - yz * (zx * ww - zw * wx) + yw * (zx * wz - zz * wx));
		det += xz * (yx * (zy * ww - zw * wy) - yy * (zx * ww - zw * wx) + yw * (zx * wy - zy * wx));
		det -= xw * (yx * (zy * wz - zz * wy) - yy * (zx * wz - zz * wx) + yz * (zx * wy - zy * wx));

		T invDet = static_cast<T>(1) / det;
		return MatrixType(+(yy * (zz * ww - wz * zw) - yz * (zy * ww - wy * zw) + yw * (zy * wz - wy * zz)) * invDet,
			-(xy * (zz * ww - wz * zw) - xz * (zy * ww - wy * zw) + xw * (zy * wz - wy * zz)) * invDet,
			+(xy * (yz * ww - wz * yw) - xz * (yy * ww - wy * yw) + xw * (yy * wz - wy * yz)) * invDet,
			-(xy * (yz * zw - zz * yw) - xz * (yy * zw - zy * yw) + xw * (yy * zz - zy * yz)) * invDet,
			-(yx * (zz * ww - wz * zw) - yz * (zx * ww - wx * zw) + yw * (zx * wz - wx * zz)) * invDet,
			+(xx * (zz * ww - wz * zw) - xz * (zx * ww - wx * zw) + xw * (zx * wz - wx * zz)) * invDet,
			-(xx * (yz * ww - wz * yw) - xz * (yx * ww - wx * yw) + xw * (yx * wz - wx * yz)) * invDet,
			+(xx * (yz * zw - zz * yw) - xz * (yx * zw - zx * yw) + xw * (yx * zz - zx * yz)) * invDet,
			+(yx * (zy * ww - wy * zw) - yy * (zx * ww - wx * zw) + yw * (zx * wy - wx * zy)) * invDet,
			-(xx * (zy * ww - wy * zw) - xy * (zx * ww - wx * zw) + xw * (zx * wy - wx * zy)) * invDet,
			+(xx * (yy * ww - wy * yw) - xy * (yx * ww - wx * yw) + xw * (yx * wy - wx * yy)) * invDet,
			-(xx * (yy * zw - zy * yw) - xy * (yx * zw - zx * yw) + xw * (yx * zy - zx * yy)) * invDet,
			-(yx * (zy * wz - wy * zz) - yy * (zx * wz - wx * zz) + yz * (zx * wy - wx * zy)) * invDet,
			+(xx * (zy * wz - wy * zz) - xy * (zx * wz - wx * zz) + xz * (zx * wy - wx * zy)) * invDet,
			-(xx * (yy * wz - wy * yz) - xy * (yx * wz - wx * yz) + xz * (yx * wy - wx * yy)) * invDet,
			+(xx * (yy * zz - zy * yz) - xy * (yx * zz - zx * yz) + xz * (yx * zy - zx * yy)) * invDet);
	}
	
	MatrixType Transpose() const
	{
		if constexpr (3 == Rows && 3 == Cols)
		{
			return MatrixType(Data(0), Data(3), Data(6),
							  Data(1), Data(4), Data(7),
							  Data(2), Data(5), Data(8));
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return MatrixType(Data(0), Data(4), Data(8), Data(12),
							  Data(1), Data(5), Data(9), Data(13),
							  Data(2), Data(6), Data(10), Data(14),
							  Data(3), Data(7), Data(11), Data(15));
		}
	}

	// Returns main diagonal vector.
	TVector<T, Cols> Diagonal() const
	{
		static_assert(Cols == Rows);

		if constexpr(3 == Rows && 3 == Cols)
		{
			return TVector<T, Cols>(Data(0), Data(4), Data(8));
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return TVector<T, Cols>(Data(0), Data(5), Data(10), Data(15));
		}
	}

	CD_FORCEINLINE T Trace() const { return Diagonal().Sum(); }

	// Extract translation vector from affine matrix.
	CD_FORCEINLINE TVector<T, Cols - 1> GetTranslation() const
	{
		static_assert(Rows >= 3 && Cols >= 3);
		if constexpr (3 == Rows && 3 == Cols)
		{
			return TVector<T, 2>(data[2][0], data[2][1]);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return TVector<T, 3>(data[3][0], data[3][1], data[3][2]);
		}
	}
	
	// Extract scale vector from affine matrix.
	CD_FORCEINLINE TVector<T, Cols - 1> GetScale() const
	{
		static_assert(Rows >= 3 && Cols >= 3);
		if constexpr (3 == Rows && 3 == Cols)
		{
			return TVector<T, 3>(data[0].Length(), data[1].Length());
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return TVector<T, 3>(data[0].Length(), data[1].Length(), data[2].Length());
		}
	}

	// Extract rotation matrix from affine matrix.
	TMatrix<T, Rows - 1, Cols - 1> GetRotation() const
	{
		static_assert(Rows >= 3 && Cols >= 3);

		if constexpr (3 == Rows && 3 == Cols)
		{
			T sx = data[0].Length();
			T sy = data[1].Length();

			return TMatrix<T, 2, 2>(Data(0) / sx, Data(1) / sx,
									Data(3) / sy, Data(4) / sy);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			T sx = data[0].Length();
			T sy = data[1].Length();
			T sz = data[2].Length();

			return TMatrix<T, 3, 3>(Data(0) / sx, Data(1) / sx, Data(2) / sx,
									Data(4) / sy, Data(5) / sy, Data(6) / sy,
									Data(8) / sz, Data(9) / sz, Data(10) / sz);
		}
	}

	// Operators
	CD_FORCEINLINE MatrixType operator+(T value) const { return MatrixType(*this) += value; }
	MatrixType& operator+=(T value)
	{
		int index = 0;
		std::for_each(Begin(), End(), [&value, &index](T& component)
		{
			component += value;
			++index;
		});
		
		return *this;
	}
	
	CD_FORCEINLINE MatrixType operator+(const MatrixType& rhs) const { return MatrixType(*this) += rhs; }
	MatrixType& operator+=(const MatrixType& rhs)
	{
		int index = 0;
		std::for_each(Begin(), End(), [&rhs, &index](T& component)
		{
			component += rhs.Data(index);
			++index;
		});
		
		return *this;
	}		

	CD_FORCEINLINE MatrixType operator-(T value) const { return (*this) + (-value); }
	CD_FORCEINLINE MatrixType& operator-=(T value) { return (*this) += (-value); }

	CD_FORCEINLINE MatrixType operator-(const MatrixType& rhs) const { return MatrixType(*this) -= rhs; }
	MatrixType& operator-=(const MatrixType& rhs)
	{
		int index = 0;
		std::for_each(Begin(), End(), [&rhs, &index](T& component)
		{
			component -= rhs.Data(index);
			++index;
		});
		
		return *this;
	}	
	
	CD_FORCEINLINE MatrixType operator*(T value) const { return MatrixType(*this) *= value; }
	MatrixType& operator*=(T value)
	{
		int index = 0;
		std::for_each(Begin(), End(), [&value, &index](T& component)
		{
			component *= value;
			++index;
		});

		return *this;
	}

	TVector<T, Cols> operator*(const TVector<T, Rows>& v) const
	{
		// TODO : glm has a register based optimization on it.
		if constexpr (3 == Rows && 3 == Cols)
		{
			return TVector<T, Cols>(
				Data(0) * v.x() + Data(3) * v.y() + Data(6) * v.z(),
				Data(1) * v.x() + Data(4) * v.y() + Data(7) * v.z(),
				Data(2) * v.x() + Data(5) * v.y() + Data(8) * v.z());
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return TVector<T, Cols>(
				Data(0) * v.x() + Data(4) * v.y() + Data(8)  * v.z() + Data(12) * v.w(),
				Data(1) * v.x() + Data(5) * v.y() + Data(9)  * v.z() + Data(13) * v.w(),
				Data(2) * v.x() + Data(6) * v.y() + Data(10) * v.z() + Data(14) * v.w(),
				Data(3) * v.x() + Data(7) * v.y() + Data(11) * v.z() + Data(15) * v.w());
		}
	}

	MatrixType operator*(const MatrixType& rhs) const
	{
		if constexpr (3 == Rows && 3 == Cols)
		{
			return MatrixType(Data(0) * rhs.Data(0) + Data(3) * rhs.Data(1) + Data(6) * rhs.Data(2),
							  Data(1) * rhs.Data(0) + Data(4) * rhs.Data(1) + Data(7) * rhs.Data(2),
							  Data(2) * rhs.Data(0) + Data(5) * rhs.Data(1) + Data(8) * rhs.Data(2),
							  Data(0) * rhs.Data(3) + Data(3) * rhs.Data(4) + Data(6) * rhs.Data(5),
							  Data(1) * rhs.Data(3) + Data(4) * rhs.Data(4) + Data(7) * rhs.Data(5),
							  Data(2) * rhs.Data(3) + Data(5) * rhs.Data(4) + Data(8) * rhs.Data(5),
							  Data(0) * rhs.Data(6) + Data(3) * rhs.Data(7) + Data(6) * rhs.Data(8),
							  Data(1) * rhs.Data(6) + Data(4) * rhs.Data(7) + Data(7) * rhs.Data(8),
							  Data(2) * rhs.Data(6) + Data(5) * rhs.Data(7) + Data(8) * rhs.Data(8));
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return MatrixType(Data(0) * rhs.Data(0)  + Data(4) * rhs.Data(1)  + Data(8)  * rhs.Data(2)  + Data(12) * rhs.Data(3),
							  Data(1) * rhs.Data(0)  + Data(5) * rhs.Data(1)  + Data(9)  * rhs.Data(2)  + Data(13) * rhs.Data(3),
							  Data(2) * rhs.Data(0)  + Data(6) * rhs.Data(1)  + Data(10) * rhs.Data(2)  + Data(14) * rhs.Data(3),
							  Data(3) * rhs.Data(0)  + Data(7) * rhs.Data(1)  + Data(11) * rhs.Data(2)  + Data(15) * rhs.Data(3),
							  Data(0) * rhs.Data(4)  + Data(4) * rhs.Data(5)  + Data(8)  * rhs.Data(6)  + Data(12) * rhs.Data(7),
							  Data(1) * rhs.Data(4)  + Data(5) * rhs.Data(5)  + Data(9)  * rhs.Data(6)  + Data(13) * rhs.Data(7),
							  Data(2) * rhs.Data(4)  + Data(6) * rhs.Data(5)  + Data(10) * rhs.Data(6)  + Data(14) * rhs.Data(7),
							  Data(3) * rhs.Data(4)  + Data(7) * rhs.Data(5)  + Data(11) * rhs.Data(6)  + Data(15) * rhs.Data(7),
							  Data(0) * rhs.Data(8)  + Data(4) * rhs.Data(9)  + Data(8)  * rhs.Data(10) + Data(12) * rhs.Data(11),
							  Data(1) * rhs.Data(8)  + Data(5) * rhs.Data(9)  + Data(9)  * rhs.Data(10) + Data(13) * rhs.Data(11),
							  Data(2) * rhs.Data(8)  + Data(6) * rhs.Data(9)  + Data(10) * rhs.Data(10) + Data(14) * rhs.Data(11),
							  Data(3) * rhs.Data(8)  + Data(7) * rhs.Data(9)  + Data(11) * rhs.Data(10) + Data(15) * rhs.Data(11),
							  Data(0) * rhs.Data(12) + Data(4) * rhs.Data(13) + Data(8)  * rhs.Data(14) + Data(12) * rhs.Data(15),
							  Data(1) * rhs.Data(12) + Data(5) * rhs.Data(13) + Data(9)  * rhs.Data(14) + Data(13) * rhs.Data(15),
							  Data(2) * rhs.Data(12) + Data(6) * rhs.Data(13) + Data(10) * rhs.Data(14) + Data(14) * rhs.Data(15),
							  Data(3) * rhs.Data(12) + Data(7) * rhs.Data(13) + Data(11) * rhs.Data(14) + Data(15) * rhs.Data(15));
		}
	}

	CD_FORCEINLINE MatrixType operator/(T value) const { return (*this) * (1 / value); }
	CD_FORCEINLINE MatrixType& operator/=(T value) { return (*this) *= (1 / value); }

private:
	TVector<T, Rows> data[Cols];
};

using Matrix3x3 = TMatrix<float, 3, 3>;
using Matrix4x4 = TMatrix<float, 4, 4>;

static_assert(9 * sizeof(float) == sizeof(Matrix3x3));
static_assert(16 * sizeof(float) == sizeof(Matrix4x4));

static_assert(std::is_standard_layout_v<Matrix3x3> && std::is_trivial_v<Matrix3x3>);
static_assert(std::is_standard_layout_v<Matrix4x4> && std::is_trivial_v<Matrix4x4>);

}