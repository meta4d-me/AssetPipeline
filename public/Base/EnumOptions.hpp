#pragma

#include "Base/NameOf.h"

#include <cstring> // std::memset
#include <type_traits>

namespace cd
{

template<typename T>
class EnumOptions
{
public:
	static_assert(std::is_enum<T>());
	using ValueType = std::underlying_type<T>::type;
	static constexpr size_t EnumCount = nameof::enum_count<T>();
	static constexpr size_t Alignment = 4;
	static constexpr size_t AlignedCount = EnumCount + Alignment - EnumCount % Alignment;

public:
	EnumOptions()
	{
		std::memset(m_options, 0, AlignedCount);
	}

	EnumOptions(const EnumOptions&) = default;
	EnumOptions& operator=(const EnumOptions&) = default;
	EnumOptions(EnumOptions&&) = default;
	EnumOptions& operator=(EnumOptions&&) = default;
	~EnumOptions() = default;

	bool IsEnabled(T e) const { return 1 == m_options[static_cast<ValueType>(e)]; }
	void Enable(T e) { m_options[static_cast<ValueType>(e)] = 1; }
	void Disable(T e) { m_options[static_cast<ValueType>(e)] = 0; }

private:
	// Can be more memory optimized by bits operation. But require more careful operations and extra calculate time cost.
	std::uint8_t m_options[AlignedCount];
};

}