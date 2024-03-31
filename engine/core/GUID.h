#pragma once

#include <array>
#include <string>
#include <functional>

namespace nimo
{
class GUID
{
public:
    static GUID Create();
    GUID();
    explicit GUID(const std::array<unsigned char, 16> &bytes);
	explicit GUID(std::array<unsigned char, 16> &&bytes);
    explicit GUID(const std::string& view);

    GUID(const GUID &other) = default;
	GUID &operator=(const GUID &other) = default;
	GUID(GUID &&other) = default;
	GUID &operator=(GUID &&other) = default;
    ~GUID()= default;

	bool operator==(const GUID &other) const;
	bool operator!=(const GUID &other) const;

	std::string Str() const;
	operator std::string() const;
	const std::array<unsigned char, 16>& bytes() const;
	void swap(GUID &other);
	bool Valid() const;
    void invalidate();

private:
    std::array<unsigned char, 16> m_bytes;
};
};

namespace std
{
	template <>
	struct hash<nimo::GUID>
	{
		std::size_t operator()(nimo::GUID const &obj) const
		{
			return std::hash<std::string>{}(obj.Str());
		}
	};
}