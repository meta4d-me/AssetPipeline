#pragma once

#include <stdint.h>

namespace cd
{

// Due to non-universal support of C++23, simply implement byte swap here
template<typename T, size_t sz>
struct swap_bytes
{
	inline T operator()(T val)
	{
		throw std::out_of_range("T data size");
	}
};

// byte_swap for 1 byte
template<typename T>
struct swap_bytes<T, 1>
{
	inline T operator()(T val)
	{
		return val;
	}
};

// byte_swap for 2 bytes
template<typename T>
struct swap_bytes<T, 2>
{
	inline T operator()(T val)
	{
		return ((((val) >> 8) & 0xff) | (((val) & 0xff) << 8));
	}
};

// byte_swap for 4 bytes
template<typename T>
struct swap_bytes<T, 4>
{
	inline T operator()(T val)
	{
		return ((((val) & 0xff000000) >> 24) | (((val) & 0x00ff0000) >> 8) | (((val) & 0x0000ff00) << 8) | (((val) & 0x000000ff) << 24));
	}
};

// byte_swap for float
template<>
struct swap_bytes<float, 4>
{
	inline float operator()(float val)
	{
		uint32_t mem = swap_bytes<uint32_t, sizeof(uint32_t)>()(*(reinterpret_cast<uint32_t*>(&val)));
		return *(reinterpret_cast<float*>(&mem));
	}
};

// byte_swap for 8 bytes
template<typename T>
struct swap_bytes<T, 8>
{
	inline T operator()(T val)
	{
		return ((((val) & 0xff00000000000000ull) >> 56) | (((val) & 0x00ff000000000000ull) >> 40) | (((val) & 0x0000ff0000000000ull) >> 24) | (((val) & 0x000000ff00000000ull) >> 8) |
			   (((val) & 0x00000000ff000000ull) << 8) | (((val) & 0x0000000000ff0000ull) << 24) | (((val) & 0x000000000000ff00ull) << 40) | (((val) & 0x00000000000000ffull) << 56));
	}
};

// double
template<>
struct swap_bytes<double, 8>
{
	inline double operator()(double val)
	{
		uint64_t mem = swap_bytes<uint64_t, sizeof(uint64_t)>()(*(reinterpret_cast<uint64_t*>(&val)));
		return *(reinterpret_cast<double*>(&mem));
	}
};

template<typename T>
inline T byte_swap(T value)
{
	static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
	static_assert(std::is_arithmetic_v<T>);
	return swap_bytes<T, sizeof(T)>()(value);
}

}