#include "Base/Endian.h"

namespace cd
{

Endian::Endian()
{
    m_native = isLittleEndian() ? LITTLE_ENDIAN : BIG_ENDIAN;
}

Endian::~Endian()
{

}

const uint8_t Endian::GetNative()
{
    static Endian instance;
    return instance.m_native;
}


const bool Endian::isLittleEndian() const
{
    union {
        uint32_t i;
        char c[4];
    } bint = { 0x04030201 };
    return bint.c[0] == 0x01;
}

} // namespace cd
