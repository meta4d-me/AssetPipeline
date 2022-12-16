#pragma once

#include <istream>

namespace cd
{

// InputArchive reads data from any classes inherited from std::istream, such as ifstream, iostream to write to reference parameter.
// The performance of reading binary data is much more important than OutputArchive so we don't want to use SwapBytes in engine runtime.
// SwapBytes controls if it will swap byte order
template<bool SwapBytesOrder>
class TInputArchive
{
public:
	TInputArchive() = delete;
	explicit TInputArchive(std::istream* pIStream) : m_pIStream(pIStream) {}
	TInputArchive(const TInputArchive&) = delete;
	TInputArchive& operator=(const TInputArchive&) = delete;
	TInputArchive(TInputArchive&&) = delete;
	TInputArchive& operator=(TInputArchive&&) = delete;
	~TInputArchive() = default;

	TInputArchive& operator>>(uint8_t& data) { return Import(data); }
	TInputArchive& operator>>(uint16_t& data) { return Import(data); }
	TInputArchive& operator>>(uint32_t& data) { return Import(data); }
	TInputArchive& operator>>(uint64_t& data) { return Import(data); }
	TInputArchive& operator>>(int8_t& data) { return Import(data); }
	TInputArchive& operator>>(int16_t& data) { return Import(data); }
	TInputArchive& operator>>(int32_t& data) { return Import(data); }
	TInputArchive& operator>>(int64_t& data) { return Import(data); }
	TInputArchive& operator>>(float& data) { return Import(data); }
	TInputArchive& operator>>(double& data) { return Import(data); }
	TInputArchive& operator>>(char& data) { return Import(data); }
	TInputArchive& operator>>(std::string& data) { return Import(data); }

	template<typename T>
	void ImportBuffer(T data)
	{
		static_assert(std::is_pointer_v<T> && "Data buffer should be pointer.");
		size_t bufferBytes;
		m_pIStream->read(reinterpret_cast<char*>(&bufferBytes), sizeof(size_t));
		if constexpr (SwapBytesOrder)
		{
			bufferBytes = std::byteswap(bufferBytes);
		}
		m_pIStream->read(reinterpret_cast<char*>(data), bufferBytes);
	}

public:
	template<typename T>
	TInputArchive& Import(T& data)
	{
		if constexpr (std::is_integral_v<T>)
		{
			m_pIStream->read(reinterpret_cast<char*>(&data), sizeof(data));
			if constexpr (SwapBytesOrder)
			{
				data = std::byteswap(data);
			}
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			m_pIStream->read(reinterpret_cast<char*>(&data), sizeof(data));
			if constexpr (4 == sizeof(T))
			{
				data = std::byteswap(static_cast<uint32_t>(data));
			}
			else if constexpr (8 == sizeof(T))
			{
				data = std::byteswap(static_cast<uint64_t>(data));
			}
			else
			{
				static_assert("80 bit long double or half float?");
			}
		}
		else if constexpr (std::is_same<T, std::string>())
		{
			size_t dataLength;
			m_pIStream->read(reinterpret_cast<char*>(&dataLength), sizeof(size_t));
			if constexpr (SwapBytesOrder)
			{
				dataLength = std::byteswap(dataLength);
			}
			data.resize(dataLength);
			m_pIStream->read(reinterpret_cast<char*>(data.data()), dataLength);
		}
		else
		{
			static_assert("Unsupported import data types, check usages or add implementation for it.");
		}

		return *this;
	}

private:
	std::istream* m_pIStream;
};

using InputArchive = TInputArchive<false>;
using InputArchiveSwapBytes = TInputArchive<false>;

}