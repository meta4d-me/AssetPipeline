#pragma once

#include <cstdint>
#include "Base/Export.h"

namespace cd
{

enum class EndianType : uint8_t
{
    LittelEndian = 0,
    BigEndian = 1,
};

class CORE_API Endian final
{
public:
    Endian(const Endian&) = delete;
    Endian& operator=(const Endian&) = delete;
    Endian(Endian&&) = delete;
    Endian& operator=(Endian&&) = delete;

    static const EndianType GetNative();

private:
    Endian();
    ~Endian();

    const bool IsLittleEndian() const;

    EndianType m_native;
};

} // namespace cd
