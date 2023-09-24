#pragma once

#include "Base/Template.h"

#include <cassert>
#include <optional>

namespace cd
{

// TODO : consider T is not built-in types.
template<typename T, std::size_t N = 16>
class DynamicArray
{
public:
	DynamicArray() = default;
	DynamicArray(const DynamicArray& rhs)
	{
		*this = rhs;
	}

	DynamicArray& operator=(const DynamicArray& rhs)
	{
		Clear();
		for (std::size_t i = 0; i < rhs.m_size; ++i)
		{
			Add(rhs.Get(i));
		}

		return *this;
	}

	DynamicArray(DynamicArray&& rhs)
	{
		*this = rhs;
	}

	DynamicArray& operator=(DynamicArray&& rhs)
	{
		// TODO : optimize
		Clear();
		for (std::size_t i = 0; i < rhs.m_size; ++i)
		{
			Add(rhs.Get(i));
		}
		return *this;
	}

	~DynamicArray()
	{
		if (m_pData != m_stackData)
		{
			delete[] m_pData;
			m_pData = nullptr;
		}
	}

	CD_FORCEINLINE T Get(std::size_t index) const { return m_pData[index]; }
	CD_FORCEINLINE T operator[](std::size_t index) const { return m_pData[index]; }
	CD_FORCEINLINE T& operator[](std::size_t index) { return m_pData[index]; }
	CD_FORCEINLINE std::size_t Size() const { return m_size; }
	CD_FORCEINLINE bool Empty() const { return m_size == 0; }
	CD_FORCEINLINE void Clear() { m_size = 0; }

	CD_FORCEINLINE T* begin() { return &m_pData[0]; }
	CD_FORCEINLINE T* end() { return &m_pData[m_size]; }
	CD_FORCEINLINE const T* begin() const { return &m_pData[0]; }
	CD_FORCEINLINE const T* end() const { return &m_pData[m_size]; }

	bool Contains(T value) const
	{
		for (std::size_t i = 0; i < m_size; ++i)
		{
			if (m_pData[i] == value)
			{
				return true;
			}
		}

		return false;
	}

	std::optional<std::size_t> GetIndex(T value) const
	{
		for (std::size_t i = 0; i < m_size; ++i)
		{
			if (m_pData[i] == value)
			{
				return i;
			}
		}

		return std::nullopt;
	}

	void Add(T data)
	{
		if (m_size >= m_capcity)
		{
			T* pOld = m_pData;
			m_capcity *= 2;
			m_pData = new T[m_capcity];

			for (std::size_t i = 0; i < m_size; ++i)
			{
				m_pData[i] = pOld[i];
			}

			if (pOld != m_stackData)
			{
				delete[] pOld;
			}
		}

		m_pData[m_size++] = data;
	}

	void RemoveByIndex(std::size_t index)
	{
		assert(index < m_size);
		m_pData[index] = m_pData[--m_size];
	}

	void RemoveByValue(T value)
	{
		for (std::size_t i = 0; i < m_size; ++i)
		{
			if (m_pData[i] == value)
			{
				RemoveByIndex(i);
				break;
			}
		}
	}

private:
	std::size_t m_size = 0U;
	std::size_t m_capcity = N;
	T m_stackData[N];
	T* m_pData = m_stackData;
};

}