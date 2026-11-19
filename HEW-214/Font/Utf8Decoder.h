#pragma once
#include <cstdint>
#include <string>

class Utf8Decoder
{
public:
    static bool Decode(
        const std::string& utf8,
        std::u32string& outText);
};