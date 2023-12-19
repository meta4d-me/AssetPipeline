#pragma

#include "Base/NameOf.h"

#include <vector>

namespace cd
{

//
// BitFlags avoid memory management and manipulate operations on bits.
// Convenient to process bits on/off combined with enum class.
//
template<typename T>
class BitFlags
{
public:
	static_assert(std::is_enum_v<T>);
	// Fixed size of bits calcualted in compile time.
	static constexpr size_t EnumCount = nameof::enum_count<T>();

public:
	BitFlags() { m_bits.resize(EnumCount, false); }
	BitFlags(const BitFlags&) = default;
	BitFlags& operator=(const BitFlags&) = default;
	BitFlags(BitFlags&&) = default;
	BitFlags& operator=(BitFlags&&) = default;
	~BitFlags() = default;

	bool IsEnabled(T e) const { return m_bits[static_cast<size_t>(e)]; }
	void Enable(T e) { m_bits[static_cast<size_t>(e)] = true; }
	void Disable(T e) { m_bits[static_cast<size_t>(e)] = false; }
	bool operator!=(const BitFlags<T>& rhs) { return m_bits != rhs.m_bits; }
	bool operator==(const BitFlags<T>& rhs) { return m_bits == rhs.m_bits; }

private:
	std::vector<bool> m_bits;
};

}