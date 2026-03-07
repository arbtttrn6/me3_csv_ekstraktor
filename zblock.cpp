#include "zblock.h"

/*--------------------------------------------------------------------
ZBlock -- функции для работы с разжатием pcc-файла
--------------------------------------------------------------------*/
namespace ZBlock
{
const uint32_t SwapU32(uint32_t aValue)
{
	return __builtin_bswap32(aValue);
}

std::string BinaryReader::wideStringToUTF8(const std::wstring& wideStr)
{
	std::string result;
	for (wchar_t wc : wideStr)
	{
		if (wc == 0) { break; };
		if (wc < 0x80)
		{
			result += static_cast<char>(wc);
		}
		else if (wc < 0x800)
		{
			result += static_cast<char>(0xC0 | (wc >> 6));
			result += static_cast<char>(0x80 | (wc & 0x3F));
		}
		else
		{
			result += static_cast<char>(0xE0 | (wc >> 12));
			result += static_cast<char>(0x80 | ((wc >> 6) & 0x3F));
			result += static_cast<char>(0x80 | (wc & 0x3F));
		};
	};
	return result;
};

std::string BinaryReader::utf16_to_utf8(const std::u16string& u16str)
{
	std::string result;

	for (size_t i = 0; i < u16str.length(); ++i)
	{
		char16_t c = u16str[i];

		if (c >= 0xD800 && c <= 0xDBFF && i + 1 < u16str.length())
		{
			char16_t high = c;
			char16_t low = u16str[i + 1];

			if (low >= 0xDC00 && low <= 0xDFFF)
			{
				uint32_t codePoint = 0x10000 + ((high - 0xD800) << 10) + (low - 0xDC00);

				if (codePoint <= 0x7F)
				{
					result += static_cast<char>(codePoint);
				}
				else if (codePoint <= 0x7FF)
				{
					result += static_cast<char>(0xC0 | (codePoint >> 6));
					result += static_cast<char>(0x80 | (codePoint & 0x3F));
				}
				else if (codePoint <= 0xFFFF)
				{
					result += static_cast<char>(0xE0 | (codePoint >> 12));
					result += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
					result += static_cast<char>(0x80 | (codePoint & 0x3F));
				}
				else
				{
					result += static_cast<char>(0xF0 | (codePoint >> 18));
					result += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F));
					result += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
					result += static_cast<char>(0x80 | (codePoint & 0x3F));
				}

				i++;
				continue;
			}
		}

		if (c <= 0x7F)
		{
			result += static_cast<char>(c);
		}
		else if (c <= 0x7FF)
		{
			result += static_cast<char>(0xC0 | (c >> 6));
			result += static_cast<char>(0x80 | (c & 0x3F));
		}
		else
		{
			result += static_cast<char>(0xE0 | (c >> 12));
			result += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
			result += static_cast<char>(0x80 | (c & 0x3F));
		}
	}

	return result;
};

void BinaryReader::checkBounds(size_t required) const
{
	if (Position + required > Buffer.size())
	{
		throw std::runtime_error("Чтение за концом буфера: Position=" +
								 std::to_string(Position) + ", required=" +
								 std::to_string(required) + ", available=" +
								 std::to_string(Buffer.size() - Position));
	};
};

void BinaryReader::seek(size_t pos)
{
	if (pos > Buffer.size())
	{
		throw std::runtime_error("Поиск за концом буфера: " + std::to_string(pos) + " > " + std::to_string(Buffer.size()));
	}
	Position = pos;
};

size_t BinaryReader::tell() const { return Position; };
size_t BinaryReader::remaining() const { return Buffer.size() - Position; };

uint32_t BinaryReader::readU32()
{
	if (Position + 4 > Buffer.size())
	{
		throw std::runtime_error("Чтение за концом буфера Position=" +
								 std::to_string(Position) + ", available=" +
								 std::to_string(Buffer.size() - Position));
	}
	uint32_t value;
	memcpy(&value, Buffer.data() + Position, 4);
	Position += 4;

	return value;
};

int32_t BinaryReader::readS32()
{
	checkBounds(4);
	int32_t value;
	memcpy(&value, Buffer.data() + Position, 4);
	Position += 4;

	return value;
};

std::string BinaryReader::readAsciiString(size_t length)
{
	if (Position + length > Buffer.size())
	{
		throw std::runtime_error("Чтение за концом данных");
	};

	std::string result(reinterpret_cast<const char*>(Buffer.data() + Position), length);
	Position += length;

	return result;
};

std::string BinaryReader::readUnicodeString(size_t charCount)
{
	size_t byteLength = charCount * 2;
	if (Position + byteLength > Buffer.size())
	{
		throw std::runtime_error("Чтение за концом данных");
	};

	// Конвертация UTF-16 в UTF-8
	//std::wstring wideStr(charCount, L'\0');
	//memcpy(wideStr.data(), Buffer.data() + Position, byteLength);
	std::u16string u16str(charCount, u'\0');
	memcpy(u16str.data(), Buffer.data() + Position, byteLength);

	Position += byteLength;

	return utf16_to_utf8(u16str);
};

void BinaryReader::readBytes(uint8_t* output, size_t count)
{
	checkBounds(count);
	memcpy(output, Buffer.data() + Position, count);
	Position += count;
};

std::vector<uint8_t> decompress(const std::vector<uint8_t>& Buffer, size_t offset, size_t count)
{
	offset = 0;
	count = 0;

	if (Buffer.empty())
	{
		throw std::invalid_argument("Буфер пуст");
	}

	if (count == 0)
	{
		count = Buffer.size() - offset;
	}

	if (offset + count > Buffer.size())
	{
		throw std::out_of_range("Offset + count за границами буфера");
	}

	// Создание reader для работы с буфером
	ZBlock::BinaryReader reader(Buffer);
	reader.seek(offset);

	// Чтение Magic number
	uint32_t MagicStream = reader.readU32();
	if (MagicStream != ZBlock::Magic && SwapU32(MagicStream) != ZBlock::Magic)
	{
		throw std::runtime_error("Найден неправильный блок zlib");
	}

	// Чтение размера сегмента
	uint32_t buffMaxSegmentSize = reader.readU32();
	if (buffMaxSegmentSize != ZBlock::MaxSegmentSize)
	{
		throw std::runtime_error("Неверный размер сегмента");
	}

	// Чтение размеров
	uint32_t totComprSize = reader.readU32();
	uint32_t totUncomprSize = reader.readU32();

	// Подготовка выходного буфера
	std::vector<uint8_t> outputBuffer(totUncomprSize);

	// Расчет количества сегментов
	int numOfSegm = static_cast<int>(std::ceil(static_cast<double>(totUncomprSize) / ZBlock::MaxSegmentSize));
	int headSegm = 16; // Позиция после заголовка (4 * uint32_t)
	int dataSegm = headSegm + (numOfSegm * 8);
	size_t buffOff = 0;

	for (int i = 0; i < numOfSegm; i++)
	{
		// Чтение информации о сегменте
		reader.seek(headSegm);
		int comprSegm = reader.readS32();
		int uncomprSegm = reader.readS32();
		headSegm = reader.tell();

		// Переход к данным сегмента
		reader.seek(dataSegm);

		// Разжатие с помощью zlib
		std::vector<uint8_t> compressedSegment(comprSegm);
		reader.readBytes(compressedSegment.data(), comprSegm);

		// Подготовка буфера для распакованных данных
		std::vector<uint8_t> uncompressedSegment(uncomprSegm);

		z_stream zstream;
		memset(&zstream, 0, sizeof(zstream));
		zstream.next_in = compressedSegment.data();
		zstream.avail_in = compressedSegment.size();
		zstream.next_out = uncompressedSegment.data();
		zstream.avail_out = uncompressedSegment.size();

		// Инициализация inflate для zlib
		if (inflateInit(&zstream) != Z_OK)
		{
			throw std::runtime_error("Ошибка инициализации zlib inflate");
		}

		// Разжатие
		int result = inflate(&zstream, Z_FINISH);
		if (result != Z_STREAM_END && result != Z_OK)
		{
			inflateEnd(&zstream);
			throw std::runtime_error("Ошибка разжатия сегмента");
		}

		// Завершение работы
		inflateEnd(&zstream);

		// Копирование распакованных данных в выходной буфер
		if (buffOff + uncomprSegm > outputBuffer.size())
		{
			throw std::runtime_error("Разжатые данные выше ожидаемого размера");
		}
		memcpy(outputBuffer.data() + buffOff, uncompressedSegment.data(), uncomprSegm);

		buffOff += uncomprSegm;
		dataSegm += comprSegm;
	}

	return outputBuffer;
};

// Асинхронная версия
#include <future>

std::future<std::vector<uint8_t>> decompressAsync(const std::vector<uint8_t>& Buffer, size_t offset, size_t count)
{
	offset = 0;
	count = 0;
	return std::async(std::launch::async, [=, &Buffer]()
	{
		return decompress(Buffer, offset, count);
	});
};
}
