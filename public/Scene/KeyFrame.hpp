#pragma once

#include "Math/Quaternion.hpp"

namespace cd
{

enum class KeyFrameType
{
	Translation,
	Rotation,
	Scale
};

template<typename KeyFrameValue, KeyFrameType KeyType>
class KeyFrame
{
public:
	static KeyFrameValue Identitiy()
	{
		if constexpr (KeyFrameType::Translation == KeyType)
		{
			return KeyFrameValue::Zero();
		}
		else if constexpr (KeyFrameType::Rotation == KeyType)
		{
			return KeyFrameValue::Identity();
		}
		else if constexpr (KeyFrameType::Scale == KeyType)
		{
			return KeyFrameValue::One();
		}
		else
		{
			static_assert("Unknown KeyFrameType.");
		}
	}

public:
	KeyFrame() = default;
	explicit KeyFrame(float time, KeyFrameValue value) : m_time(time), m_value(MoveTemp(value)) {}
	KeyFrame(const KeyFrame&) = default;
	KeyFrame& operator=(const KeyFrame&) = default;
	KeyFrame(KeyFrame&&) = default;
	KeyFrame& operator=(KeyFrame&&) = default;
	~KeyFrame() = default;

	void SetTime(float time) { m_time = time; }
	float GetTime() const { return m_time; }

	void SetValue(KeyFrameValue value) { m_value = MoveTemp(value); }
	KeyFrameValue& GetValue() { return m_value; }
	const KeyFrameValue& GetValue() const { return m_value; }

private:
	float m_time;
	KeyFrameValue m_value;
};

using TranslationKey = KeyFrame<Vec3f, KeyFrameType::Translation>;
using RotationKey = KeyFrame<Quaternion, KeyFrameType::Rotation>;
using ScaleKey = KeyFrame<Vec3f, KeyFrameType::Scale>;

}