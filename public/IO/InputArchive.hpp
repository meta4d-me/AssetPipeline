#pragma once

#include "Math/AxisSystem.hpp"
#include "Math/Box.hpp"
#include "Math/Matrix.hpp"
#include "Math/Transform.hpp"
#include "Utilities/ByteSwap.h"

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
	TInputArchive& operator>>(bool& data) { return Import(data); }
	TInputArchive& operator>>(std::string& data) { return Import(data); }
	TInputArchive& operator>>(Vec2f& data) { return ImportBuffer(data.begin()); }
	TInputArchive& operator>>(Vec3f& data) { return ImportBuffer(data.begin()); }
	TInputArchive& operator>>(Vec4f& data) { return ImportBuffer(data.begin()); }
	TInputArchive& operator>>(Quaternion& data) { return ImportBuffer(data.begin()); }
	TInputArchive& operator>>(Matrix3x3& data) { return ImportBuffer(data.begin()); }
	TInputArchive& operator>>(Matrix4x4& data) { return ImportBuffer(data.begin()); }
	TInputArchive& operator>>(Transform& data) { return ImportBuffer(data.begin()); }
	TInputArchive& operator>>(AABB& data) { ImportBuffer(data.Min().begin()); ImportBuffer(data.Max().begin()); return *this; }
	TInputArchive& operator>>(AxisSystem& data)
	{
		uint8_t handedness, up, front;
		Import(handedness);
		Import(up);
		Import(front);
		data.SetHandedness(static_cast<Handedness>(handedness));
		data.SetUpVector(static_cast<UpVector>(up));
		data.SetFrontVector(static_cast<FrontVector>(front));
		return *this;
	}

	uint64_t FetchBufferSize()
	{
		uint64_t bufferBytes;
		m_pIStream->read(reinterpret_cast<char*>(&bufferBytes), sizeof(uint64_t));
		if constexpr (SwapBytesOrder)
		{
			bufferBytes = byte_swap<uint64_t>(bufferBytes);
		}
		return bufferBytes;
	}

	template<typename T>
	TInputArchive& ImportBuffer(T data)
	{
		ImportBuffer(data, FetchBufferSize());
		return *this;
	}

	template<typename T>
	TInputArchive& ImportBuffer(T data, uint64_t bufferSize)
	{
		static_assert(std::is_pointer_v<T> && "Data buffer should be pointer.");
		m_pIStream->read(reinterpret_cast<char*>(data), bufferSize);

		return *this;
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
				data = byte_swap<T>(data);
			}
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			m_pIStream->read(reinterpret_cast<char*>(&data), sizeof(data));
			
			if constexpr (SwapBytesOrder)
			{
				if constexpr (4 == sizeof(T))
				{
					data = byte_swap<float>(data);
				}
				else if constexpr (8 == sizeof(T))
				{
					data = byte_swap<double>(data);
				}
				else
				{
					static_assert("80 bit long double or half float?");
				}
			}
		}
		else if constexpr (std::is_same<T, std::string>())
		{
			uint64_t dataLength;
			m_pIStream->read(reinterpret_cast<char*>(&dataLength), sizeof(uint64_t));
			if constexpr (SwapBytesOrder)
			{
				dataLength = byte_swap<uint64_t>(dataLength);
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
using InputArchiveSwapBytes = TInputArchive<true>;

}