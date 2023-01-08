#pragma once

#include "Math/Vector.hpp"

#include <cstring> // std::memset

namespace cd
{

// TMatrix majors in column order.
template<typename T, std::size_t Rows, std::size_t Cols>
class TMatrix
{
public:
	using MatrixType = TMatrix<T, Rows, Cols>;
	static constexpr std::size_t RowCount = Rows;
	static constexpr std::size_t ColCount = Cols;
	static constexpr std::size_t Size = RowCount * ColCount;
	using iterator = T*;
	using const_iterator = const T*;

	static MatrixType Identity()
	{
		if constexpr (2 == Rows && 2 == Cols)
		{
			return MatrixType(1, 0, 0, 1);
		}	
		else if constexpr (3 == Rows && 3 == Cols)
		{
			return MatrixType(1, 0, 0, 0, 1, 0, 0, 0, 1);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return MatrixType(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		}
	}

	// Transform
	static MatrixType Transform(const TVector<T, 3>& position, const TMatrix<T, 3, 3>& rotation, const TVector<T, 3>& scale)
	{
		static_assert(4 == Rows && 4 == Cols);

		// rotation
		TVector<T, 4> c0(rotation(0, 0), rotation(1, 0), rotation(2, 0), 0);
		TVector<T, 4> c1(rotation(0, 1), rotation(1, 1), rotation(2, 1), 0);
		TVector<T, 4> c2(rotation(0, 2), rotation(1, 2), rotation(2, 2), 0);
		TVector<T, 4> c3(0, 0, 0, 1);

		// scale
		c0 *= scale.x;
		c1 *= scale.y;
		c2 *= scale.z;

		// translation
		c3[0] = position.x;
		c3[1] = position.y;
		c3[2] = position.z;

		return Matrix<T, 4, 4>(cd::MoveTemp(c0), cd::MoveTemp(c1), cd::MoveTemp(c2), cd::MoveTemp(c3));
	}

	template<Handedness Hand>
	static MatrixType LookAt(const TVector<T, 3>& eye, const TVector<T, 3>& at, const TVector<T, 3>& up)
	{
		static_assert(4 == Rows && 4 == Cols);

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
			static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
	}

	template<Handedness Hand, NDCDepth NDC>
	static MatrixType Perspective(T fovy, T aspect, T near, T far)
	{
		float height = static_cast<T>(1) / std::tan(cd::DegreeToRadian<T>(fovy) * static_cast<T>(0.5));
		float width = height * 1.0f / aspect;
		float delta = far - near;

		float aa;
		float bb;
		if constexpr (NDCDepth::MinusOneToOne == NDC)
		{
			aa = (far + near) / delta;
			bb = (2.0f * far * near) / delta;
		}
		else
		{
			aa = far / delta;
			bb = near * aa;
		}

		constexpr float xx = static_cast<T>(0);
		constexpr float yy = static_cast<T>(0);
		if constexpr (Handedness::Left == Hand)
		{
			return MatrixType(width, static_cast<T>(0), -xx, static_cast<T>(0),
				static_cast<T>(0), height, -yy, static_cast<T>(0),
				static_cast<T>(0), static_cast<T>(0), aa, -bb,
				static_cast<T>(0), static_cast<T>(0), 1.0f, static_cast<T>(0));
		}
		else
		{
			return MatrixType(width, static_cast<T>(0), xx, static_cast<T>(0),
				static_cast<T>(0), height, yy, static_cast<T>(0),
				static_cast<T>(0), static_cast<T>(0), -aa, -bb,
				static_cast<T>(0), static_cast<T>(0), -1.0f, static_cast<T>(0));
		}
	}

	static MatrixType Perspective(T fovy, T aspect, T near, T far, bool isNDCDepthHomogeneous)
	{
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
		float deltaX = right - left;
		float deltaY = top - bottom;
		float deltaZ = far - near;

		T aa = static_cast<T>(2) / deltaX;
		T bb = static_cast<T>(2) / deltaY;
		T dd = (left + right) / -deltaX;
		T ee = (top + bottom) / -deltaY;

		T cc;
		T ff;
		if constexpr (NDCDepth::MinusOneToOne == NDC)
		{
			cc = static_cast<T>(2) / deltaZ;
			ff = (near + far) / -deltaZ;
		}
		else
		{
			cc = static_cast<T>(1) / deltaZ;
			ff = near / -deltaZ;
		}

		if constexpr (Handedness::Left == Hand)
		{
			return MatrixType(aa, static_cast<T>(0), static_cast<T>(0), dd,
				static_cast<T>(0), bb, static_cast<T>(0), ee,
				static_cast<T>(0), static_cast<T>(0), cc, ff,
				static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
		}
		else
		{
			return MatrixType(aa, static_cast<T>(0), static_cast<T>(0), dd,
				static_cast<T>(0), bb, static_cast<T>(0), ee,
				static_cast<T>(0), static_cast<T>(0), -cc, ff,
				static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
		}
	}

	static MatrixType Orthographic(T left, T right, T top, T bottom, T near, T far, T offset, bool isNDCDepthHomogeneous)
	{
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
	
	// Rows x Cols
	TMatrix(const T* const pRawData)
	{
		int index = 0;
		std::for_each(Begin(), End(), [&pRawData, &index](T& component) { component = pRawData[index++]; });
	}
	
	// 2x2
	TMatrix(T a00, T a10, T a01, T a11)
	{
		static_assert(2 == Rows && 2 == Cols);
		data[0] = TVector<T, 2>(a00, a10);
		data[1] = TVector<T, 2>(a01, a11);
	}

	// 2x2
	TMatrix(TVector<T, 2> colVec0, TVector<T, 2> colVec1)
	{
		static_assert(2 == Rows && 2 == Cols);
		data[0] = cd::MoveTemp(colVec0);
		data[1] = cd::MoveTemp(colVec1);
	}	
	
	// 3x3
	TMatrix(T a00, T a10, T a20,
		T a01, T a11, T a21,
		T a02, T a12, T a22)
	{
		static_assert(3 == Rows && 3 == Cols);
		data[0] = TVector<T, 3>(a00, a10, a20);
		data[1] = TVector<T, 3>(a01, a11, a21);
		data[2] = TVector<T, 3>(a02, a12, a22);
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
	TMatrix(T a00, T a10, T a20, T a30,
		T a01, T a11, T a21, T a31,
		T a02, T a12, T a22, T a32,
		T a03, T a13, T a23, T a33)
	{
		static_assert(4 == Rows && 4 == Cols);
		data[0] = TVector<T, 4>(a00, a10, a20, a30);
		data[1] = TVector<T, 4>(a01, a11, a21, a31);
		data[2] = TVector<T, 4>(a02, a12, a22, a32);
		data[3] = TVector<T, 4>(a03, a13, a23, a33);
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

	CD_FORCEINLINE iterator Begin() { return &data[0][0]; }
	CD_FORCEINLINE iterator End() { return &data[0][0] + Size; }
	CD_FORCEINLINE const_iterator Begin() const { return &data[0][0]; }
	CD_FORCEINLINE const_iterator End() const { return &data[0][0] + Size; }

	// Get
	CD_FORCEINLINE const TVector<T, Rows>& GetColumn(int index) const { return data[index]; }
	CD_FORCEINLINE TVector<T, Rows>& GetColumn(int index) { return data[index]; }
	CD_FORCEINLINE T operator()(int row, int col) const { return data[col][row]; }
	CD_FORCEINLINE T& operator()(int row, int col) { return data[col][row]; }
	CD_FORCEINLINE T Data(int index) const { return reinterpret_cast<const T*>(data)[index]; }
	CD_FORCEINLINE T& Data(int index) { return reinterpret_cast<T*>(data)[index]; }

	// Clear
	void Clear() { std::memset(Begin(), static_cast<T>(0), Size); }

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

	// Translation
	CD_FORCEINLINE TVector<T, Cols> GetTranslation() const
	{
		if constexpr (3 == Rows && 3 == Cols)
		{
			return TVector<T, 2>(data[2][0], data[2][1]);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return TVector<T, 3>(data[3][0], data[3][1], data[3][2]);
		}
	}
	
	CD_FORCEINLINE MatrixType GetTranslationMatrix() const
	{
		if constexpr (3 == Rows && 3 == Cols)
		{
			return MatrixType(1, 0, 0, 0, 1, 0, data[2][0], data[2][1], 1);
		}
		else if constexpr (4 == Rows && 4 == Cols)
		{
			return MatrixType(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, data[3][0], data[3][1], data[3][2], 1);
		}
	}
	
	// Scale
	CD_FORCEINLINE TVector<T, Cols> GetScale() const
	{
		static_assert(Rows >= 3 && Cols >= 3);
		return TVector<T, 3>(GetColumn(0).Length(), GetColumn(1).Length(), GetColumn(2).Length());	
	}

	// Rotation
	static CD_FORCEINLINE MatrixType RotationX(T angle) { return RotationX(TVector<T, 2>(std::cos(angle), std::sin(angle))); }
	static CD_FORCEINLINE MatrixType RotationY(T angle) { return RotationY(TVector<T, 2>(std::cos(angle), std::sin(angle))); }
	static CD_FORCEINLINE MatrixType RotationZ(T angle) { return RotationZ(TVector<T, 2>(std::cos(angle), std::sin(angle))); }
	
	static CD_FORCEINLINE MatrixType RotationX(const TVector<T, 2>& v)
	{
		if constexpr(3 == Rows && 3 == Cols)
		{
			return MatrixType(1, 0, 0, 0, v.x(), v.y(), 0, -v.y(), v.x());
		}
		else if constexpr(4 == Rows && 4 == Cols)
		{
			return MatrixType(1, 0, 0, 0, 0, v.x(), v.y(), 0, 0, -v.y(), v.x(), 0, 0, 0, 0, 1);
		}
	}
	
	static CD_FORCEINLINE MatrixType RotationY(const TVector<T, 2>& v)
	{
		if constexpr(3 == Rows && 3 == Cols)
		{
			return MatrixType(v.x(), 0, -v.y(), 0, 1, 0, v.y(), 0, v.x());
		}
		else if constexpr(4 == Rows && 4 == Cols)
		{
			return MatrixType(v.x(), 0, -v.y(), 0, 0, 1, 0, 0, v.y(), 0, v.x(), 0, 0, 0, 0, 1);
		}
	}

	static CD_FORCEINLINE MatrixType RotationZ(const TVector<T, 2>& v)
	{
		if constexpr(3 == Rows && 3 == Cols)
		{
			return MatrixType(v.x(), v.y(), 0, -v.y(), v.x(), 0, 0, 0, 1);
		}
		else if constexpr(4 == Rows && 4 == Cols)
		{
			return MatrixType(v.x(), v.y(), 0, 0, -v.y(), v.x(), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		}
	}	
	
	// Operators
	MatrixType operator+(T value) const
	{
		MatrixType result;
		
		int index = 0;
		std::for_each(Begin(), End(), [&value, &result, &index](T& component)
		{
			result.Data(index) = component + value;
			++index;
		});
		
		return result;
	}
	
	MatrixType& operator+=(T value) const
	{
		int index = 0;
		std::for_each(Begin(), End(), [&value, &index](T& component)
		{
			component += value;
			++index;
		});
		
		return *this;
	}
	
	MatrixType operator+(const MatrixType& rhs) const
	{
		MatrixType result;
		
		int index = 0;
		std::for_each(Begin(), End(), [&rhs, &result, &index](T& component)
		{
			result.Data(index) = component + rhs.Data(index);
			++index;
		});
		
		return result;
	}
	
	MatrixType& operator+=(const MatrixType& rhs) const
	{
		int index = 0;
		std::for_each(Begin(), End(), [&rhs, &index](T& component)
		{
			component += rhs.Data(index);
			++index;
		});
		
		return *this;
	}		
	
	CD_FORCEINLINE MatrixType operator-(T value) const { return (*this) += (-value); }
	CD_FORCEINLINE MatrixType& operator-=(T value) const { return (*this) += (-value); }
	
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
	
	MatrixType operator-(const MatrixType& rhs) const
	{
		MatrixType result;
		
		int index = 0;
		std::for_each(Begin(), End(), [&rhs, &result, &index](T& component)
		{
			result.Data(index) = component - rhs.Data(index);
			++index;
		});
		
		return result;
	}
	
	MatrixType& operator-=(const MatrixType& rhs) const
	{
		int index = 0;
		std::for_each(Begin(), End(), [&rhs, &index](T& component)
		{
			component -= rhs.Data(index);
			++index;
		});
		
		return *this;
	}	
	
	MatrixType operator*(const MatrixType& rhs) const
	{
		MatrixType result;
		if constexpr(2 == Rows && 2 == Cols)
		{
			result.Data(0) = Data(0) * rhs.Data(0) + Data(2) * rhs.Data(1);
			result.Data(1) = Data(1) * rhs.Data(0) + Data(3) * rhs.Data(1);
			result.Data(2) = Data(0) * rhs.Data(2) + Data(2) * rhs.Data(3);
			result.Data(3) = Data(1) * rhs.Data(2) + Data(3) * rhs.Data(3);
		}
		else if constexpr(3 == Rows && 3 == Cols)
		{
			{
				TVector<T, 3> row(Data(0), Data(3), Data(6));
				result.Data[0] = rhs.GetColumn(0).DotProduct(row);
				result.Data[3] = rhs.GetColumn(1).DotProduct(row);
				result.Data[6] = rhs.GetColumn(2).DotProduct(row);
			}
			
			{
				TVector<T, 3> row(Data(1), Data(4), Data(7));
				result.Data[1] = rhs.GetColumn(0).DotProduct(row);
				result.Data[4] = rhs.GetColumn(1).DotProduct(row);
				result.Data[7] = rhs.GetColumn(2).DotProduct(row);
			}

			{
				TVector<T, 3> row(Data(2), Data(5), Data(8));
				result.Data[2] = rhs.GetColumn(0).DotProduct(row);
				result.Data[5] = rhs.GetColumn(1).DotProduct(row);
				result.Data[8] = rhs.GetColumn(2).DotProduct(row);
			}			
		}
		else if constexpr(4 == Rows && 4 == Cols)
		{
			{
				TVector<T, 4> row(Data(0), Data(4), Data(8), Data(12));
				result.Data[0] = rhs.GetColumn(0).DotProduct(row);
				result.Data[4] = rhs.GetColumn(1).DotProduct(row);
				result.Data[8] = rhs.GetColumn(2).DotProduct(row);
				result.Data[12] = rhs.GetColumn(3).DotProduct(row);
			}
			
			{
				TVector<T, 4> row(Data(1), Data(5), Data(9), Data(13));
				result.Data[1] = rhs.GetColumn(0).DotProduct(row);
				result.Data[5] = rhs.GetColumn(1).DotProduct(row);
				result.Data[9] = rhs.GetColumn(2).DotProduct(row);
				result.Data[13] = rhs.GetColumn(3).DotProduct(row);
			}

			{
				TVector<T, 4> row(Data(2), Data(6), Data(10), Data(14));
				result.Data[2] = rhs.GetColumn(0).DotProduct(row);
				result.Data[6] = rhs.GetColumn(1).DotProduct(row);
				result.Data[10] = rhs.GetColumn(2).DotProduct(row);
				result.Data[14] = rhs.GetColumn(3).DotProduct(row);
			}

			{
				TVector<T, 4> row(Data(3), Data(7), Data(11), Data(15));
				result.Data[3] = rhs.GetColumn(0).DotProduct(row);
				result.Data[7] = rhs.GetColumn(1).DotProduct(row);
				result.Data[11] = rhs.GetColumn(2).DotProduct(row);
				result.Data[15] = rhs.GetColumn(3).DotProduct(row);
			}			
		}
		
		return result;
	}
	
	MatrixType operator*(T value) const
	{
		MatrixType result;
		
		int index = 0;
		std::for_each(Begin(), End(), [&value, &result, &index](T& component)
		{
			result.Data(index) = component * value;
			++index;
		});
		
		return result;		
	}
	
	MatrixType& operator*=(T value) const
	{
		int index = 0;
		std::for_each(Begin(), End(), [&value, &index](T& component)
		{
			component *= value;
			++index;
		});
		
		return *this;		
	}
	
	CD_FORCEINLINE MatrixType operator/(T value) const { return (*this) *= (1 / value); }
	CD_FORCEINLINE MatrixType& operator/=(T value) const { return (*this) *= (1 / value); }

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