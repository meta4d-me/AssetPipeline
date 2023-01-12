#include "Base/Endian.h"

namespace cd
{

Endian::Endian()
{
    m_native = IsLittleEndian()
        ? EndianType::LittelEndian
        : EndianType::BigEndian;
}

Endian::~Endian()
{

}

const EndianType Endian::GetNative()
{
    static Endian instance;
    return instance.m_native;
}


const bool Endian::IsLittleEndian() const
{
    union {
        uint32_t i;
        char c[4];
    } bint = { 0x04030201 };
    return bint.c[0] == 0x01;
}

} // namespace cd
