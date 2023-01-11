#pragma once

#include <ostream>

#include "Utilities/ByteSwap.h"

namespace cd
{

// OutputArchive read data from parameter to write to any classes inherited from std::ostream, such as ofstream, iostream.
// SwapBytes controls if it will swap byte order
template<bool SwapBytesOrder>
class TOutputArchive
{
public:
	TOutputArchive() = delete;
	explicit TOutputArchive(std::ostream* pOStream) : m_pOStream(pOStream) {}
	TOutputArchive(const TOutputArchive&) = delete;
	TOutputArchive& operator=(const TOutputArchive&) = delete;
	TOutputArchive(TOutputArchive&&) = delete;
	TOutputArchive& operator=(TOutputArchive&&) = delete;
	~TOutputArchive() = default;

	TOutputArchive& operator<<(uint8_t data) { return Export(data); }
	TOutputArchive& operator<<(uint16_t data) { return Export(data); }
	TOutputArchive& operator<<(uint32_t data) { return Export(data); }
	TOutputArchive& operator<<(uint64_t data) { return Export(data); }
	TOutputArchive& operator<<(int8_t data) { return Export(data); }
	TOutputArchive& operator<<(int16_t data) { return Export(data); }
	TOutputArchive& operator<<(int32_t data) { return Export(data); }
	TOutputArchive& operator<<(int64_t data) { return Export(data); }
	TOutputArchive& operator<<(float data) { return Export(data); }
	TOutputArchive& operator<<(double data) { return Export(data); }
	TOutputArchive& operator<<(char data) { return Export(data); }
	TOutputArchive& operator<<(const std::string& data) { return Export(data); }

	template<typename T>
	TOutputArchive& ExportBuffer(T data, std::size_t size)
	{
		static_assert(std::is_pointer_v<T> && "Data buffer should be a pointer.");
		uint64_t sourceBufferBytes = static_cast<uint64_t>(size * sizeof(std::remove_pointer_t<T>));
		uint64_t bufferBytes;
		if constexpr (SwapBytesOrder)
		{
			bufferBytes = byte_swap<uint64_t>(sourceBufferBytes);
		}
		else
		{
			bufferBytes = sourceBufferBytes;
		}

		m_pOStream->write(reinterpret_cast<const char *>(&bufferBytes), sizeof(uint64_t));
		m_pOStream->write(reinterpret_cast<const char *>(data), sourceBufferBytes);

		return *this;
	}

private:
	template<typename T>
	TOutputArchive& Export(const T& data)
	{
		if constexpr (std::is_integral_v<T>)
		{
			if constexpr (SwapBytesOrder)
			{
				T checkedData = byte_swap<T>(data);
				m_pOStream->write(reinterpret_cast<const char*>(&checkedData), sizeof(T));
			}
			else
			{
				m_pOStream->write(reinterpret_cast<const char*>(&data), sizeof(T));
			}
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			if constexpr (SwapBytesOrder)
			{
				if constexpr (4 == sizeof(T))
				{
					float checkedData = byte_swap<float>(data);
					m_pOStream->write(reinterpret_cast<const char*>(&checkedData), sizeof(T));
				}
				else if constexpr (8 == sizeof(T))
				{
					double checkedData = byte_swap<double>(data);
					m_pOStream->write(reinterpret_cast<const char*>(&checkedData), sizeof(T));
				}
				else
				{
					static_assert("80 bit long double or half float?");
				}
			}
			else
			{
				m_pOStream->write(reinterpret_cast<const char*>(&data), sizeof(T));
			}
		}
		else if constexpr (std::is_same<T, std::string>())
		{
			uint64_t dataLength = data.size();
			if constexpr (SwapBytesOrder)
			{
				// std::string is just array of 1 byte char so don't need to swap bytes.
				dataLength = byte_swap<uint64_t>(dataLength);
			}

			m_pOStream->write(reinterpret_cast<const char*>(&dataLength), sizeof(uint64_t));
			m_pOStream->write(data.c_str(), data.size());
		}
		else
		{
			// std::wstring?
			static_assert("Unsupported export data types, check usages or add implementation for it.");
		}

		return *this;
	}

private:
	std::ostream* m_pOStream;
};

using OutputArchive = TOutputArchive<false>;
using OutputArchiveSwapBytes = TOutputArchive<true>;

}