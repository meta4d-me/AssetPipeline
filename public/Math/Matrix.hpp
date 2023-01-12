#pragma once

#include "Math/Vector.hpp"

namespace cd
{

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

		if constexpr (2 == Rows && 2 == Cols)
		{
			return MatrixType(one, zero, zero, one);
		}	
		else if constexpr (3 == Rows && 3 == Cols)
		{
			return MatrixType(one, zero, zero, zero, one, zero, zero, zero, one);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return MatrixType(one, zero, zero, zero, zero, one, zero, zero, zero, zero, one, zero, zero, zero, zero, one);
		}
	}

	static MatrixType Transform(const TVector<T, 3>& translation, const TMatrix<T, 3, 3>& rotation, const TVector<T, 3>& scale)
	{
		static_assert(4 == Rows && 4 == Cols);
		
		// rotation
		TVector<T, 4> c0(rotation(0, 0), rotation(1, 0), rotation(2, 0), 0);
		TVector<T, 4> c1(rotation(0, 1), rotation(1, 1), rotation(2, 1), 0);
		TVector<T, 4> c2(rotation(0, 2), rotation(1, 2), rotation(2, 2), 0);
		TVector<T, 4> c3(0, 0, 0, 1);
		
		// scale
		c0 *= scale.x();
		c1 *= scale.y();
		c2 *= scale.z();
		
		// translation
		c3[0] = translation.x();
		c3[1] = translation.y();
		c3[2] = translation.z();
		
		return MatrixType(cd::MoveTemp(c0), cd::MoveTemp(c1), cd::MoveTemp(c2), cd::MoveTemp(c3));
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

		return MatrixType(right.x(), right.y(), right.z(), -right.Dot(eye),
						  upDirection.x(), upDirection.y(), upDirection.z(), -upDirection.Dot(eye),
						  view.x(), view.y(), view.z(), -view.Dot(eye),
						  zero, zero, zero, one);
	}

	template<Handedness Hand, NDCDepth NDC>
	static MatrixType Perspective(T fovy, T aspect, T near, T far)
	{
		static_assert(4 == Rows && 4 == Cols);
		constexpr T zero = static_cast<T>(0);
		constexpr T half = static_cast<T>(0.5);
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T height = one / std::tan(cd::DegreeToRadian<T>(fovy) * half);
		T width = height * one / aspect;
		T delta = far - near;

		T aa;
		T bb;
		if constexpr (NDCDepth::MinusOneToOne == NDC)
		{
			aa = (far + near) / delta;
			bb = (two * far * near) / delta;
		}
		else
		{
			aa = far / delta;
			bb = near * aa;
		}

		constexpr T xx = zero;
		constexpr T yy = zero;
		if constexpr (Handedness::Left == Hand)
		{
			return MatrixType(width, zero, -xx, zero,
							  zero, height, -yy, zero,
							  zero, zero, aa, -bb,
							  zero, zero, one, zero);
		}
		else
		{
			return MatrixType(width, zero, xx, zero,
							  zero, height, yy, zero,
							  zero, zero, -aa, -bb,
							  zero, zero, -one, zero);
		}
	}

	static MatrixType Perspective(T fovy, T aspect, T near, T far, bool isNDCDepthHomogeneous)
	{
		static_assert(4 == Rows && 4 == Cols);
		if (isNDCDepthHomogeneous)
		{
			return MatrixType::Perspective<cd::Handedness::Left, cd::NDCDepth::MinusOneToOne>(fovy, aspect, near, far);
		}
		else
		{
			return MatrixType::Perspective<cd::Handedness::Left, cd::NDCDepth::ZeroToOne>(fovy, aspect, near, far);
		}
	}

	template<Handedness Hand, NDCDepth NDC>
	static MatrixType Orthographic(T left, T right, T top, T bottom, T near, T far, T offset)
	{
		static_assert(4 == Rows && 4 == Cols);
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T deltaX = right - left;
		T deltaY = top - bottom;
		T deltaZ = far - near;

		T aa = two / deltaX;
		T bb = two / deltaY;
		T dd = (left + right) / -deltaX;
		T ee = (top + bottom) / -deltaY;

		T cc;
		T ff;
		if constexpr (NDCDepth::MinusOneToOne == NDC)
		{
			cc = two / deltaZ;
			ff = (near + far) / -deltaZ;
		}
		else
		{
			cc = one / deltaZ;
			ff = near / -deltaZ;
		}

		if constexpr (Handedness::Left == Hand)
		{
			return MatrixType(aa, zero, zero, dd,
							  zero, bb, zero, ee,
							  zero, zero, cc, ff,
							  zero, zero, zero, one);
		}
		else
		{
			return MatrixType(aa, zero, zero, dd,
							  zero, bb, zero, ee,
							  zero, zero, -cc, ff,
							  zero, zero, zero, one);
		}
	}

	static MatrixType Orthographic(T left, T right, T top, T bottom, T near, T far, T offset, bool isNDCDepthHomogeneous)
	{
		static_assert(4 == Rows && 4 == Cols);
		if (isNDCDepthHomogeneous)
		{
			return MatrixType::Orthographic<cd::Handedness::Left, cd::NDCDepth::MinusOneToOne>(left, right, top, bottom, near, far, offset);
		}
		else
		{
			return MatrixType::Orthographic<cd::Handedness::Left, cd::NDCDepth::ZeroToOne>(left, right, top, bottom, near, far, offset);
		}
	}

public:
	// Default uninitialized.
	TMatrix() = default;

	// 2x2
	TMatrix(T a00, T a01, T a02, T a03)
	{
		static_assert(2 == Rows && 2 == Cols);
		data[0] = TVector<T, 2>(a00, a01);
		data[1] = TVector<T, 2>(a02, a03);
	}

	// 2x2
	TMatrix(TVector<T, 2> colVec0, TVector<T, 2> colVec1)
	{
		static_assert(2 == Rows && 2 == Cols);
		data[0] = cd::MoveTemp(colVec0);
		data[1] = cd::MoveTemp(colVec1);
	}	
	
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
	CD_FORCEINLINE T operator()(int row, int col) const { return data[col][row]; }
	CD_FORCEINLINE T& operator()(int row, int col) { return data[col][row]; }
	CD_FORCEINLINE T Data(int index) const { return reinterpret_cast<const T*>(data)[index]; }
	CD_FORCEINLINE T& Data(int index) { return reinterpret_cast<T*>(data)[index]; }

	// Clear
	void Clear() { std::memset(Begin(), 0, Size); }

	// Math
	MatrixType Inverse() const
	{
		MatrixType result;
		if constexpr (2 == Rows && 2 == Cols)
		{
			T determinant = Data(0) * Data(3) - Data(1) * Data(2);
			T inverseDeterminant = 1 / determinant;
			result.Data(0) = inverseDeterminant * Data(3);
			result.Data(1) = -inverseDeterminant * Data(1);
			result.Data(2) = -inverseDeterminant * Data(2);
			result.Data(3) = inverseDeterminant * Data(0);
		}
		else if constexpr (3 == Rows && 3 == Cols)
		{
			T sub11 = Data(4) * Data(8) - Data(5) * Data(7);
			T sub12 = -Data(1) * Data(8) + Data(2) * Data(7);
			T sub13 = Data(1) * Data(5) - Data(2) * Data(4);
			T determinant = Data(0) * sub11 + Data(3) * sub12 + Data(6) * sub13;
			
			// Find determinants of 2x2 submatrices for the elements of the inverse.
			result.Data(0) = sub11 / determinant;
			result.Data(1) = sub12 / determinant;
			result.Data(2) = sub13 / determinant;
			result.Data(3) = (Data(6) * Data(5) - Data(3) * Data(8)) / determinant;
			result.Data(4) = (Data(0) * Data(8) - Data(6) * Data(2)) / determinant;
			result.Data(5) = (Data(3) * Data(2) - Data(0) * Data(5)) / determinant;
			result.Data(6) = (Data(3) * Data(7) - Data(6) * Data(4)) / determinant;
			result.Data(7) = (Data(6) * Data(1) - Data(0) * Data(7)) / determinant;
			result.Data(8) = (Data(0) * Data(4) - Data(3) * Data(1)) / determinant;
		}
		
		return result;
	}
	
	MatrixType Transpose() const
	{
		if constexpr (2 == Rows && 2 == Cols)
		{
			return MatrixType(Data(0), Data(2),
							  Data(1), Data(3));
		}
		else if constexpr (3 == Rows && 3 == Cols)
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

	MatrixType HadamardProduct(const MatrixType& rhs) const
	{
		MatrixType result;
		
		int index = 0;
		std::for_each(Begin(), End(), [&rhs, &result, &index](T& component)
		{
			result.Data(index) = Data(index) * rhs.Data(index);
			++index;
		});
		
		return result;
	}

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
									Data(7) / sz, Data(8) / sz, Data(9) / sz);
		}
	}

	// Operators
	CD_FORCEINLINE MatrixType operator+() const { return *this; }
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
	
	MatrixType operator-() const
	{
		MatrixType result;

		int index = 0;
		std::for_each(Begin(), End(), [&result, &index](T& component)
			{
				result.Data(index) = component * static_cast<T>(-1);
				++index;
			});

		return result;
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

	CD_FORCEINLINE MatrixType operator*(const MatrixType& rhs) const { return MatrixType(*this) *= rhs; }
	MatrixType& operator*=(const MatrixType& rhs)
	{
		if constexpr (2 == Rows && 2 == Cols)
		{
			Data(0) = Data(0) * rhs.Data(0) + Data(2) * rhs.Data(1);
			Data(1) = Data(1) * rhs.Data(0) + Data(3) * rhs.Data(1);
			Data(2) = Data(0) * rhs.Data(2) + Data(2) * rhs.Data(3);
			Data(3) = Data(1) * rhs.Data(2) + Data(3) * rhs.Data(3);
		}
		else if constexpr (3 == Rows && 3 == Cols)
		{
			TVector<T, 3> row0(Data(0), Data(3), Data(6));
			Data(0) = rhs.GetColumn(0).Dot(row0);
			Data(3) = rhs.GetColumn(1).Dot(row0);
			Data(6) = rhs.GetColumn(2).Dot(row0);

			TVector<T, 3> row1(Data(1), Data(4), Data(7));
			Data(1) = rhs.GetColumn(0).Dot(row1);
			Data(4) = rhs.GetColumn(1).Dot(row1);
			Data(7) = rhs.GetColumn(2).Dot(row1);

			TVector<T, 3> row2(Data(2), Data(5), Data(8));
			Data(2) = rhs.GetColumn(0).Dot(row2);
			Data(5) = rhs.GetColumn(1).Dot(row2);
			Data(8) = rhs.GetColumn(2).Dot(row2);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			TVector<T, 4> row0(Data(0), Data(4), Data(8), Data(12));
			Data(0) = rhs.GetColumn(0).Dot(row0);
			Data(4) = rhs.GetColumn(1).Dot(row0);
			Data(8) = rhs.GetColumn(2).Dot(row0);
			Data(12) = rhs.GetColumn(3).Dot(row0);

			TVector<T, 4> row1(Data(1), Data(5), Data(9), Data(13));
			Data(1) = rhs.GetColumn(0).Dot(row1);
			Data(5) = rhs.GetColumn(1).Dot(row1);
			Data(9) = rhs.GetColumn(2).Dot(row1);
			Data(13) = rhs.GetColumn(3).Dot(row1);

			TVector<T, 4> row2(Data(2), Data(6), Data(10), Data(14));
			Data(2) = rhs.GetColumn(0).Dot(row2);
			Data(6) = rhs.GetColumn(1).Dot(row2);
			Data(10) = rhs.GetColumn(2).Dot(row2);
			Data(14) = rhs.GetColumn(3).Dot(row2);

			TVector<T, 4> row3(Data(3), Data(7), Data(11), Data(15));
			Data(3) = rhs.GetColumn(0).Dot(row3);
			Data(7) = rhs.GetColumn(1).Dot(row3);
			Data(11) = rhs.GetColumn(2).Dot(row3);
			Data(15) = rhs.GetColumn(3).Dot(row3);
		}

		return *this;
	}

	CD_FORCEINLINE MatrixType operator/(T value) const { return (*this) * (1 / value); }
	CD_FORCEINLINE MatrixType& operator/=(T value) { return (*this) *= (1 / value); }

private:
	TVector<T, Rows> data[Cols];
};

using Matrix2x2 = TMatrix<float, 2, 2>;
using Matrix3x3 = TMatrix<float, 3, 3>;
using Matrix4x4 = TMatrix<float, 4, 4>;

static_assert(4 * sizeof(float) == sizeof(Matrix2x2));
static_assert(9 * sizeof(float) == sizeof(Matrix3x3));
static_assert(16 * sizeof(float) == sizeof(Matrix4x4));

static_assert(std::is_standard_layout_v<Matrix2x2> && std::is_trivial_v<Matrix2x2>);
static_assert(std::is_standard_layout_v<Matrix3x3> && std::is_trivial_v<Matrix3x3>);
static_assert(std::is_standard_layout_v<Matrix4x4> && std::is_trivial_v<Matrix4x4>);

}