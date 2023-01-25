#pragma once

#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"

namespace cd
{

template<typename T>
class TTransform final
{
public:
	using ValueType = T;
	static constexpr std::size_t Size = 10;
	using Iterator = T*;
	using ConstIterator = const T*;

	static TTransform Identity()
	{
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);
		return TTransform(TVector<T, 3>(zero, zero, zero),
			TQuaternion<T>(one, zero, zero, zero),
			TVector<T, 3>(one, one, one));
	}

public:
	TTransform() = default;

	TTransform(TVector<T, 3> translation, TQuaternion<T> rotation, TVector<T, 3> scale) :
		m_translation(cd::MoveTemp(translation)),
		m_rotation(cd::MoveTemp(rotation)),
		m_scale(cd::MoveTemp(scale))
	{
	}

	TTransform(const TTransform&) = default;
	TTransform& operator=(const TTransform&) = default;
	TTransform(TTransform&&) = default;
	TTransform& operator=(TTransform&&) = default;
	~TTransform() = default;

	void Clear()
	{
		m_translation.Clear();
		m_rotation.Clear();
		m_scale.Clear();
	}

	// Get
	CD_FORCEINLINE Iterator Begin() { return &m_translation[0]; }
	CD_FORCEINLINE Iterator End() { return &m_translation[0] + Size; }
	CD_FORCEINLINE ConstIterator Begin() const { return &m_translation[0]; }
	CD_FORCEINLINE ConstIterator End() const { return &m_translation[0] + Size; }

	CD_FORCEINLINE void SetTranslation(TVector<T, 3> translation) { m_translation = cd::MoveTemp(translation); }
	CD_FORCEINLINE TVector<T, 3>& GetTranslation() { return m_translation; }
	CD_FORCEINLINE const TVector<T, 3>& GetTranslation() const { return m_translation; }

	CD_FORCEINLINE void SetRotation(TQuaternion<T> rotation) { m_rotation = cd::MoveTemp(rotation); }
	CD_FORCEINLINE TQuaternion<T>& GetRotation() { return m_rotation; }
	CD_FORCEINLINE const TQuaternion<T>& GetRotation() const { return m_rotation; }

	CD_FORCEINLINE void SetScale(TVector<T, 3> scale) { m_scale = cd::MoveTemp(scale); }
	CD_FORCEINLINE TVector<T, 3>& GetScale() { return m_scale; }
	CD_FORCEINLINE const TVector<T, 3>& GetScale() const { return m_scale; }

	TMatrix<T, 4, 4> GetMatrix() const
	{
		TMatrix<T, 4, 4> result = m_rotation.ToMatrix4x4();
		result.GetColumn(3)[0] = m_translation.x();
		result.GetColumn(3)[1] = m_translation.y();
		result.GetColumn(3)[2] = m_translation.z();
		result.GetColumn(0)[0] = result.GetColumn(0).Length() * m_scale.x();
		result.GetColumn(1)[1] = result.GetColumn(1).Length() * m_scale.y();
		result.GetColumn(2)[2] = result.GetColumn(2).Length() * m_scale.z();
		
		return result;
	}

private:
	TVector<T, 3> m_translation;
	TQuaternion<T> m_rotation;
	TVector<T, 3> m_scale;
};

using Transform = TTransform<float>;

static_assert(10 * sizeof(float) == sizeof(Transform));
static_assert(std::is_standard_layout_v<Transform>&& std::is_trivial_v<Transform>);

}