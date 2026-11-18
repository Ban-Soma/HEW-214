#pragma once
#include <cstdint>

constexpr uint32_t MakeTag(char a, char b, char c, char d)
{
	return (uint32_t(uint8_t(a)) << 24) |
		(uint32_t(uint8_t(b)) << 16) |
		(uint32_t(uint8_t(c)) << 8) |
		uint32_t(uint8_t(d));
}
namespace Tag
{
    constexpr uint32_t HEAD = MakeTag('h', 'e', 'a', 'd');
    constexpr uint32_t MAXP = MakeTag('m', 'a', 'x', 'p');
    constexpr uint32_t GLYF = MakeTag('g', 'l', 'y', 'f');
    constexpr uint32_t LOCA = MakeTag('l', 'o', 'c', 'a');
    constexpr uint32_t CMAP = MakeTag('c', 'm', 'a', 'p');
    constexpr uint32_t HHEA = MakeTag('h', 'h', 'e', 'a');
    constexpr uint32_t HMTX = MakeTag('h', 'm', 't', 'x');
}