#pragma once

#include <stdint.h>

namespace cd
{

namespace
{

constexpr uint8_t LITTLE_ENDIAN = 0x00;
constexpr uint8_t BIG_ENDIAN = 0x01;

}

class Endian
{
public:
    Endian(const Endian&) = delete;
    Endian& operator=(const Endian&) = delete;
    Endian(Endian&&) = delete;
    Endian& operator=(Endian&&) = delete;

    static const uint8_t GetNative();

private:
    Endian();
    ~Endian();

    const bool isLittleEndian() const;

    uint8_t m_native;
};

} // namespace cd
