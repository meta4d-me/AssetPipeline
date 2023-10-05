#include <cstdint>

namespace cd
{

// Combines two hash values to get a third.
// Note - this function is not commutative.
constexpr uint32_t HashCombine(uint32_t a, uint32_t b)
{
	return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
}

}