#include "utils.h"
#include <cstdint>
#include <cstdio>
#include <fstream>

#include "endian.h"

/*--------------------------------------------------------------------
PrintBytes -- выводит байты
--------------------------------------------------------------------*/
void PrintBytes(const wchar_t* aStr, size_t aCount)
{
	const uint8_t* Bytes = reinterpret_cast<const uint8_t*>(aStr);
	for (size_t i = 0; i < aCount * sizeof(wchar_t); ++i)
	{
		printf("%02X ", Bytes[i]);
	}
	printf("\n");
}

uint32_t ReadUInt32(const std::vector<uint8_t> &buffer, size_t offset)
{

	if (offset + 4 > buffer.size())
	{
		throw std::out_of_range("Переполнение буфера. Offset = " +
		std::to_string(offset) + ", available = " +
		std::to_string(buffer.size() - offset));
	}

	uint32_t value;
//	std::memcpy(&value, &buffer[offset], sizeof(uint32_t));
	std::memcpy(&value, &buffer[offset], 4);

	value = ((value & 0xFF) << 24) |
			(((value >> 8) & 0xFF) << 16) |
			(((value >> 16) & 0xFF) << 8) |
			((value >> 24) & 0xFF);


	return value;
}

uint32_t readU32(const std::vector<uint8_t> &Buffer, size_t Position)
{
	if (Position + 4 > Buffer.size())
	{
		throw std::runtime_error("\nЧтение за концом буфера Position=" +
		std::to_string(Position) + ", available=" +
		std::to_string(Buffer.size() - Position));
	}
	uint32_t value;
	memcpy(&value, Buffer.data() + Position, 4);
	Position += 4;

	return value;
};

int FindBinaryDataStart(const std::vector<uint8_t>& Data, const std::vector<std::string> &sPackageNames)
{
	int iStart = 0x4;

	while (iStart < Data.size() - 4)
	{
		uint32_t iNameIndex = (static_cast<uint32_t>(Data[iStart])) |
				(static_cast<uint32_t>(Data[iStart + 1]) << 8) |
				(static_cast<uint32_t>(Data[iStart + 2]) << 16) |
				(static_cast<uint32_t>(Data[iStart + 3]) << 24);

		std::string sName = "";
		if (iNameIndex < sPackageNames.size())
		{
			sName = sPackageNames[iNameIndex];
		};
		if (iNameIndex < sPackageNames.size() && strcmp(sName.c_str(), "None") == 0)
		{
			iStart += 8;
			std::cerr << "\nNone найден" << std::endl;
			return iStart;
		};

		iStart += 4;
	}

	return 0;
}

int FindLevelExport(const std::vector<std::unique_ptr<CExportEntry>>& Exports)
{
	for (int i = 0; i < Exports.size(); i++) //static_cast<int>
	{
		if (!Exports[i])
		{
			std::cerr << "\nExport " << i << " пуст" << std::endl;
			continue;
		}

		std::string sClassName = Exports[i]->GetClassName();
		std::string sObjectName = Exports[i]->GetObjectName();

		if (strcmp(sClassName.c_str(), "Level") == 0 &&
			strcmp(sObjectName.c_str(), "PersistentLevel") == 0)
		{

			std::cout << "\nУровень найден: " << sObjectName << std::endl;
			return i;
		};
	};

	std::cerr << "\nУровень не найден" << std::endl;
	return -1;
}

bool bIsSkeletalMesh(const std::string &sClassName)
{
	return  strcmp(sClassName.c_str(), "SkeletalMeshActor") == 0 ||
			strcmp(sClassName.c_str(), "SkeletalMeshComponent") == 0;
}

void ExportToCSV(CMassEffect3Package *PCC)
{

	if (PCC->GetObjectParameters().empty())
	{
		std::cerr << "\nExportToCSV: Массив с параметрами объектов пуст!" << std::endl;
		std::cerr << "\nExportToCSV: CSV-файл создан не был!" << std::endl;
	}
	else
	{
		// Задача имени файла csv
		std::string CSViOutputPath = "map_" + PCC->GetPackageName() + ".csv";
		// Задача заголовка таблице csv
		std::string CSViTableHead = "ID;Name;LocX;LocY;LocZ;RotX;RotY;RotZ;ScaleX;ScaleY;ScaleZ;Package;IsSkeletal\n";

		// Задача потока ofstream-а для нового csv-файла
		std::ofstream CSViFile(CSViOutputPath);
		// Отправка в csv-файл заголовка таблицы
		CSViFile << CSViTableHead;

		for (int i = 0; i < static_cast<int>(PCC->GetObjectParameters().size()); i++)
		{
			std::vector<ObjectParameters> Parameters = PCC->GetObjectParameters();
			CSViFile << Parameters[i].iId << ";"
					 << Parameters[i].sName.c_str() << ";"
					 << Parameters[i].fLocation[0] << ";"
					 << Parameters[i].fLocation[1] << ";"
					 << Parameters[i].fLocation[2] << ";"
					 << Parameters[i].fRotation[0] << ";"
					 << Parameters[i].fRotation[1] << ";"
					 << Parameters[i].fRotation[2] << ";"
					 << Parameters[i].fScale[0] << ";"
					 << Parameters[i].fScale[1] << ";"
					 << Parameters[i].fScale[2] << ";"
					 << Parameters[i].sPackageName << ";";
			if (Parameters[i].bIsSkeletal == false)
			{
				CSViFile << "false\n";
			}
			else
			{
				CSViFile << "true\n";
			}

			// Отладка
			std::cout << Parameters[i].iId << ";"
					  << Parameters[i].sName.c_str() << ";"
					  << Parameters[i].fLocation[0] << ";"
					  << Parameters[i].fLocation[1] << ";"
					  << Parameters[i].fLocation[2] << ";"
					  << Parameters[i].fRotation[0] << ";"
					  << Parameters[i].fRotation[1] << ";"
					  << Parameters[i].fRotation[2] << ";"
					  << Parameters[i].fScale[0] << ";"
					  << Parameters[i].fScale[1] << ";"
					  << Parameters[i].fScale[2] << ";"
					  << Parameters[i].sPackageName << ";"
					  << Parameters[i].bIsSkeletal << "\n"
					  << std::endl;
		};

		// Закрываем поток ofstream
		CSViFile.close();

		std::cout << "\nРезультат: " << CSViOutputPath << std::endl;
	};
}

void ExportMultipleToCSV(std::vector<ObjectParameters> Parameters)
{

	if (Parameters.empty())
	{
		std::cerr << "\nExportToCSV: Массив с параметрами объектов пуст!" << std::endl;
		std::cerr << "\nExportToCSV: CSV-файл создан не был!" << std::endl;
	}
	else
	{
		// Задача имени файла csv
		std::string CSViOutputPath = "map_" + std::to_string(rand() % 1000) + ".csv";
		// Задача заголовка таблице csv
		std::string CSViTableHead = "ID;Name;LocX;LocY;LocZ;RotX;RotY;RotZ;ScaleX;ScaleY;ScaleZ;Package;IsSkeletal\n";

		// Задача потока ofstream-а для нового csv-файла
		std::ofstream CSViFile(CSViOutputPath);
		// Отправка в csv-файл заголовка таблицы
		CSViFile << CSViTableHead;

		for (int i = 0; i < static_cast<int>(Parameters.size()); i++)
		{
			CSViFile << Parameters[i].iId << ";"
					 << Parameters[i].sName.c_str() << ";"
					 << Parameters[i].fLocation[0] << ";"
					 << Parameters[i].fLocation[1] << ";"
					 << Parameters[i].fLocation[2] << ";"
					 << Parameters[i].fRotation[0] << ";"
					 << Parameters[i].fRotation[1] << ";"
					 << Parameters[i].fRotation[2] << ";"
					 << Parameters[i].fScale[0] << ";"
					 << Parameters[i].fScale[1] << ";"
					 << Parameters[i].fScale[2] << ";"
					 << Parameters[i].sPackageName << ";";
			if (Parameters[i].bIsSkeletal == false)
			{
				CSViFile << "false\n";
			}
			else
			{
				CSViFile << "true\n";
			}

			// Отладка
			std::cout << Parameters[i].iId << ";"
					  << Parameters[i].sName.c_str() << ";"
					  << Parameters[i].fLocation[0] << ";"
					  << Parameters[i].fLocation[1] << ";"
					  << Parameters[i].fLocation[2] << ";"
					  << Parameters[i].fRotation[0] << ";"
					  << Parameters[i].fRotation[1] << ";"
					  << Parameters[i].fRotation[2] << ";"
					  << Parameters[i].fScale[0] << ";"
					  << Parameters[i].fScale[1] << ";"
					  << Parameters[i].fScale[2] << ";"
					  << Parameters[i].sPackageName << ";"
					  << Parameters[i].bIsSkeletal << "\n"
					  << std::endl;
		};

		// Закрываем поток ofstream
		CSViFile.close();

		std::cout << "\nРезультат: " << CSViOutputPath << std::endl;
	};
}

float ToSingle(const uint8_t *data, size_t offset)
{
	float result;
	std::memcpy(&result, data + offset, sizeof(float));
	return result;
}

float ToSingle(const std::vector<uint8_t> &data, size_t offset)
{
	return ToSingle(data.data(), offset);
}

std::string DecimalToHex(int aiDecimal)
{
	std::string sHexChars = "0123456789ABCDEF";
	std::string sHexResult;
	while (aiDecimal > 0)
	{
		sHexResult = sHexChars[aiDecimal % 16] + sHexResult;
		aiDecimal /= 16;
	}
	return sHexResult.empty() ? "0" : sHexResult;
}
