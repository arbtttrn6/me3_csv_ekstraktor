#include <vector>
#include <string>
#include <zlib.h>
#include <stdexcept>
#include <memory>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>

#include <future>
#pragma once

/*--------------------------------------------------------------------
ZBlock -- функции для работы с разжатием pcc-файла
--------------------------------------------------------------------*/
namespace ZBlock
{
const uint32_t Magic = 0x9E2A83C1;
const uint32_t MaxSegmentSize = 0x20000; // 131072 байт

const uint32_t SwapU32(uint32_t aValue);

class BinaryReader
{
private:
	const std::vector<uint8_t>& Buffer;
	size_t Position;
	bool bLittleEndian;

	std::string wideStringToUTF8(const std::wstring& wideStr);

	std::string utf16_to_utf8(const std::u16string& u16str);

	void checkBounds(size_t required) const;

public:
	BinaryReader(const std::vector<uint8_t>& buf) : Buffer(buf), Position(0) {}

	void seek(size_t pos);

	size_t tell() const;
	size_t remaining() const;

	uint32_t readU32();
	int32_t readS32();

	std::string readAsciiString(size_t length);
	std::string readUnicodeString(size_t charCount);

	void readBytes(uint8_t* output, size_t count);
};

std::vector<uint8_t> decompress(const std::vector<uint8_t>& Buffer,
								size_t offset = 0,
								size_t count = 0);

std::future<std::vector<uint8_t>> decompressAsync(const std::vector<uint8_t>& Buffer,
												  size_t offset = 0,
												  size_t count = 0);
}
