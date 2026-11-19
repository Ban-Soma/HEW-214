#include "BinaryReader.h"

BinaryReader::BinaryReader(const std::vector<uint8_t>& data)
    :m_data(data)
{
}

uint8_t BinaryReader::ReadUInt8(size_t offset) const
{
    return m_data[offset];
}

uint16_t BinaryReader::ReadUInt16(size_t offset)const
{
    return (uint16_t(m_data[offset]) << 8) |
        uint16_t(m_data[offset + 1]);
}
uint32_t BinaryReader::ReadUInt32(size_t offset)const
{
    return
        (uint32_t(m_data[offset]) << 24) |
        (uint32_t(m_data[offset + 1]) << 16) |
        (uint32_t(m_data[offset + 2]) << 8) |
        uint32_t(m_data[offset + 3]);
}

int16_t BinaryReader::ReadInt16(size_t offset)const
{
    return static_cast<int16_t>(ReadUInt16(offset));
}


int32_t BinaryReader::ReadInt32(size_t offset) const
{
    return static_cast<int32_t>(ReadUInt32(offset));
}


