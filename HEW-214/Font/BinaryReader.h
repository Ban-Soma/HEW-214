#pragma once

#include <vector>
#include <cstdint>

class BinaryReader
{
public:
	BinaryReader(const std::vector<uint8_t>& data);
	uint8_t ReadUInt8(size_t offset)const;

	uint16_t ReadUInt16(size_t offset)const;
	uint32_t ReadUInt32(size_t offset)const;

	int16_t ReadInt16(size_t offset)const;
	int32_t ReadInt32(size_t offset)const;

private:
	const std::vector<uint8_t>& m_data;
};