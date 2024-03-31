#include "GUID.h"

#include <random>
#include <iostream>
#include <iomanip>

namespace nimo{
static std::random_device randomDevice;
static std::mt19937_64 eng(randomDevice());
static std::uniform_int_distribution<uint64_t> uniformDistribution;

int char2int(char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(const char* src, char* target)
{
  while(*src && src[1])
  {
    *(target++) = char2int(*src)*16 + char2int(src[1]);
    src += 2;
  }
}
};

nimo::GUID nimo::GUID::Create()
{
    std::array<unsigned char, 16> bytes;
    *(uint64_t*)bytes.data() = uniformDistribution(eng);
    *(((uint64_t*)bytes.data())+1) = uniformDistribution(eng);
    return GUID(bytes);
}

nimo::GUID::GUID(): m_bytes{{0}}
{}
nimo::GUID::GUID(const std::array<unsigned char, 16> &bytes) : m_bytes(bytes)
{}
nimo::GUID::GUID(std::array<unsigned char, 16> &&bytes) : m_bytes(std::move(bytes))
{}
nimo::GUID::GUID(const std::string& view)
{
    std::string subString = view.substr(0, 8);
    hex2bin(subString.c_str(), (char*)m_bytes.data());
    subString = view.substr(9, 4);
    hex2bin(subString.c_str(), (char*)m_bytes.data() + 4);
    subString = view.substr(14, 4);
    hex2bin(subString.c_str(), (char*)m_bytes.data() + 6);
    subString = view.substr(19, 4);
    hex2bin(subString.c_str(), (char*)m_bytes.data() + 8);
    subString = view.substr(24, 12);
    hex2bin(subString.c_str(), (char*)m_bytes.data() + 10);
}

bool nimo::GUID::operator==(const GUID &other) const
{
    return m_bytes == other.m_bytes;
}
bool nimo::GUID::operator!=(const GUID &other) const
{
    return !((*this) == other);
}
std::string nimo::GUID::Str() const
{
    char tmp[37];
    sprintf_s(tmp, 37, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    m_bytes[0], m_bytes[1], m_bytes[2],m_bytes[3],m_bytes[4],m_bytes[5],m_bytes[6],m_bytes[7],m_bytes[8],
    m_bytes[9],m_bytes[10],m_bytes[11],m_bytes[12],m_bytes[13],m_bytes[14],m_bytes[15]);
    return {tmp};
}
nimo::GUID::operator std::string() const
{
    return Str();
}
const std::array<unsigned char, 16>& nimo::GUID::bytes() const
{
    return m_bytes;
}
void nimo::GUID::swap(GUID &other)
{
    m_bytes.swap(other.m_bytes);
}
bool nimo::GUID::Valid() const
{
    GUID empty;
    return *this != empty;
}

void nimo::GUID::invalidate()
{
    std::fill(m_bytes.begin(), m_bytes.end(), static_cast<unsigned char>(0));
}