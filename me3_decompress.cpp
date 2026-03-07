#include "me3_decompress.h"
#include "endian.h"
#include <iostream>
#include <future>
#include "zblock.h"

/*--------------------------------------------------------------------
DecompressME3 -- разжатие pcc
--------------------------------------------------------------------*/
std::vector<uint8_t> DecompressME3(std::istream& input)
{
	input.seekg(0);
	
	// Чтение magic number
	uint32_t magic;
	input.read(reinterpret_cast<char*>(&magic), 4);
	
	if (magic != 0x9E2A83C1 && __builtin_bswap32(magic) != 0x9E2A83C1)
	{
		throw std::runtime_error("not a pcc file");
	}
	
	bool endian = magic == 0x9E2A83C1 ? Endian::Little : Endian::Big;
	
	// Чтение версии
	uint16_t versionLo, versionHi;
	input.read(reinterpret_cast<char*>(&versionLo), 2);
	input.read(reinterpret_cast<char*>(&versionHi), 2);
	
	if (endian == Endian::Big)
	{
		versionLo = __builtin_bswap16(versionLo);
		versionHi = __builtin_bswap16(versionHi);
	}
	
	if (versionLo != 684 && versionHi != 194)
	{
		throw std::runtime_error("Неподдерживаемая версия pcc");
	}
	
	size_t headerSize = 8;
	
	// Пропуск 4 байт
	input.seekg(4, std::ios_base::cur);
	headerSize += 4;
	
	// Чтение длины имени папки
	int32_t folderNameLength;
	input.read(reinterpret_cast<char*>(&folderNameLength), 4);
	if (endian == Endian::Big)
	{
		folderNameLength = __builtin_bswap32(folderNameLength);
	}
	headerSize += 4;
	
	size_t folderNameByteLength = (folderNameLength >= 0) ? 
		folderNameLength : (-folderNameLength * 2);
	input.seekg(folderNameByteLength, std::ios_base::cur);
	headerSize += folderNameByteLength;
	
	size_t packageFlagsOffset = input.tellg();
	uint32_t packageFlags;
	input.read(reinterpret_cast<char*>(&packageFlags), 4);
	if (endian == Endian::Big)
	{
		packageFlags = __builtin_bswap32(packageFlags);
	}
	headerSize += 4;
	
	if ((packageFlags & 0x02000000u) == 0)
	{
		throw std::runtime_error("PCC-файл уже разжат");
	}
	
	if ((packageFlags & 8) != 0)
	{
		input.seekg(4, std::ios_base::cur);
		headerSize += 4;
	}
	
	uint32_t nameCount, nameOffset;
	input.read(reinterpret_cast<char*>(&nameCount), 4);
	input.read(reinterpret_cast<char*>(&nameOffset), 4);
	
	if (endian == Endian::Big)
	{
		nameCount = __builtin_bswap32(nameCount);
		nameOffset = __builtin_bswap32(nameOffset);
	}
	
	input.seekg(52, std::ios_base::cur);
	headerSize += 60;
	
	uint32_t generationsCount;
	input.read(reinterpret_cast<char*>(&generationsCount), 4);
	if (endian == Endian::Big)
	{
		generationsCount = __builtin_bswap32(generationsCount);
	}
	
	input.seekg(generationsCount * 12, std::ios_base::cur);
	headerSize += generationsCount * 12;
	
	input.seekg(20, std::ios_base::cur);
	headerSize += 24;
	
	uint32_t blockCount;
	input.read(reinterpret_cast<char*>(&blockCount), 4);
	if (endian == Endian::Big)
	{
		blockCount = __builtin_bswap32(blockCount);
	}
	
	size_t headBlockOff = input.tellg();
	size_t afterBlockTableOffset = headBlockOff + (blockCount * 16);
	size_t indataOffset = afterBlockTableOffset + 8;
	
	// Подготовка выходного буфера
	std::vector<uint8_t> output;
	
	// Копирование заголовка
	input.seekg(0);
	output.resize(headerSize);
	input.read(reinterpret_cast<char*>(output.data()), headerSize);
	
	// Запись нулевого количества блоков
	uint32_t zero = 0;
	size_t currentPos = output.size();
	output.resize(currentPos + 4);
	memcpy(output.data() + currentPos, &zero, 4);
	currentPos += 4;
	
	// Копирование данных после таблицы блоков
	input.seekg(afterBlockTableOffset);
	std::vector<uint8_t> afterBlockData(8);
	input.read(reinterpret_cast<char*>(afterBlockData.data()), 8);
	output.insert(output.end(), afterBlockData.begin(), afterBlockData.end());
	currentPos += 8;
	
	// Проверка на наличие дополнительного списка имен
	if ((packageFlags & 0x10000000) != 0)
	{
		size_t bytesToCopy = nameOffset - currentPos;
		std::vector<uint8_t> extraData(bytesToCopy);
		input.read(reinterpret_cast<char*>(extraData.data()), bytesToCopy);
		output.insert(output.end(), extraData.begin(), extraData.end());
	}
	
	// Разжатие блоков
	std::vector<uint32_t> uncompressedOffsets(blockCount);
	std::vector<std::vector<uint8_t>> compressedDataBlocks(blockCount);
	
	for (uint32_t i = 0; i < blockCount; i++)
	{
			input.seekg(headBlockOff);
			
		uint32_t uncompressedOffset, uncompressedSize, compressedOffset, compressedSize;
		input.read(reinterpret_cast<char*>(&uncompressedOffset), 4);
		input.read(reinterpret_cast<char*>(&uncompressedSize), 4);
		input.read(reinterpret_cast<char*>(&compressedOffset), 4);
		input.read(reinterpret_cast<char*>(&compressedSize), 4);
		
		if (endian == Endian::Big)
		{
			uncompressedOffset = __builtin_bswap32(uncompressedOffset);
			uncompressedSize = __builtin_bswap32(uncompressedSize);
			compressedOffset = __builtin_bswap32(compressedOffset);
			compressedSize = __builtin_bswap32(compressedSize);
		}
		
		headBlockOff = input.tellg();
		uncompressedOffsets[i] = uncompressedOffset;
		
		// Чтение сжатых данных
		compressedDataBlocks[i].resize(compressedSize);
		input.seekg(compressedOffset);
		input.read(reinterpret_cast<char*>(compressedDataBlocks[i].data()), compressedSize);
		
		std::cout << "Отладка: Прочитан блок " << i << ", размер: " << compressedSize << ", смещение: " << uncompressedOffset << std::endl;
	}
	
	// Запуск асинхронного разжатия с перемещением данных
	std::vector<std::future<std::vector<uint8_t>>> tasks;
	for (uint32_t i = 0; i < blockCount; i++) {
		std::cout << "Отладка: Запуск асинхронного разжатия блока " << i
				  << ", размер данных: " << compressedDataBlocks[i].size() << std::endl;
		
		tasks.push_back(
					std::async(std::launch::async,
							   [data = std::move(compressedDataBlocks[i])]() mutable
		{
			std::cout << "ZBlock::decompress: размер буфера = " << data.size() << std::endl;
			if (data.empty())
			{
				std::cout << "ZBlock::decompress: ОШИБКА - буфер пустой!" << std::endl;
				throw std::invalid_argument("Buffer is null");
			}
			return ZBlock::decompress(data);
		}
		));
	};
	
	compressedDataBlocks.clear();
	
	// Ожидание завершения
	std::vector<std::vector<uint8_t>> decompressedBlocks(blockCount);
	for (uint32_t i = 0; i < blockCount; i++)
	{
		std::cout << "Отладка: Ожидание блока " << i << std::endl;
		try
		{
			decompressedBlocks[i] = tasks[i].get();
			std::cout << "Отладка: Блок " << i << " разжат, размер: "
					  << decompressedBlocks[i].size() << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "ОШИБКА при разжатии блока " << i << ": " << e.what() << std::endl;
			throw;
		}
	}
	
	// Запись разжатых данных
	for (uint32_t i = 0; i < blockCount; i++)
	{
		size_t requiredSize = uncompressedOffsets[i] + decompressedBlocks[i].size();
		if (requiredSize > output.size())
		{
			output.resize(requiredSize);
		}
		
		memcpy(output.data() + uncompressedOffsets[i],
			   decompressedBlocks[i].data(),
			   decompressedBlocks[i].size());
	};
	
	// Обновление флагов пакета
	uint32_t newPackageFlags = packageFlags & ~0x02000000u;
	if (endian == Endian::Big)
	{
		newPackageFlags = __builtin_bswap32(newPackageFlags);
	}
	memcpy(output.data() + packageFlagsOffset, &newPackageFlags, 4);
	
	return output;
}
