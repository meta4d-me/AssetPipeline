#pragma

#include "Base/NameOf.h"

#include <type_traits>

namespace cd
{

template<typename T>
class EnumOptionArray
{
public:
	static_assert(std::is_enum<T>());
	using ValueType = std::underlying_type<T>::type;
	constexpr int EnumCount = nameof::enum_count<T>();

public:
	bool IsEnabled(T e) const { return 1 == m_options[static_cast<ValueType>(e)]; }
	void Enable(T e) const { m_options[static_cast<ValueType>(e)] = 1; }
	void Disable(T e) const { m_options[static_cast<ValueType>(e)] = 0; }

private:
	// Can be more memory optimized by bits operation. But require more careful operations and extra calculate time cost.
	std::uint8_t m_options[ElementCount];
};

}