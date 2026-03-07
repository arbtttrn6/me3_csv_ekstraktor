#include "cmasseffect3package.h"
#include "cme3exportentry.h"
#include "me3_decompress.h"
#include <fstream>
#include "objectdef.h"

std::string CMassEffect3Package::wideStringToUTF8(const std::wstring &wideStr)
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
		}
	}
	return result;
}

uint32_t CMassEffect3Package::GetFlags() const { return PackageHeader.PackageFlags; }

int CMassEffect3Package::GetNameCount() const { return PackageHeader.NameCount; }

int CMassEffect3Package::GetNameOffset() const { return PackageHeader.PointerToNameTable; }

int CMassEffect3Package::GetExportCount() const { return PackageHeader.ExportCount; }

int CMassEffect3Package::GetImportCount() const { return PackageHeader.ImportCount; }

int CMassEffect3Package::GetImportOffset() const { return PackageHeader.PointerToImportTable; }

int CMassEffect3Package::GetExportOffset() const { return PackageHeader.PointerToExportTable; }

bool CMassEffect3Package::bIsExport(int32_t aIndex)
{
	return (aIndex >= 0 && aIndex < static_cast<int32_t>(PackageExports.size()));
}

CExportEntry *CMassEffect3Package::GetExports(int32_t aIndex)
{
	return PackageExports[aIndex].get();
}

CImportEntry *CMassEffect3Package::GetImports(int32_t aIndex)
{
	return PackageImports[-aIndex+1].get(); // +1
}

//CIEntry *CMassEffect3Package::GetEntry(int32_t aIndex) const {
//	if (aIndex > 0) { // Export
//		return PackageExports[aIndex - 1].get();
//	}

//	// Import
//	return PackageImports[-aIndex + 1].get(); // -1

//	//return nullptr;
//}

CIEntry *CMassEffect3Package::GetEntry(int aIndex) const
{
	if (aIndex > 0)
	{
		int iExportIndex = aIndex - 1;

		if (iExportIndex >= 0 && iExportIndex < PackageExports.size())
		{
			return PackageExports[iExportIndex].get();
		}

	} else if (aIndex < 0)
	{
		int iImportIndex = -aIndex - 1;

		if (iImportIndex >= 0 && iImportIndex < PackageImports.size())
		{
			return PackageImports[iImportIndex].get();
		}
	}

	return nullptr;
}

bool CMassEffect3Package::IsName(int aIndex) const
{
	if (aIndex > 0)
	{
		if (aIndex >= 0 && aIndex < static_cast<int>(PackageNames.size()))
		{
			return true;
		}
	}

	if (aIndex < 0)
	{
		int iImportIndex = -aIndex - 1;
		return (iImportIndex >= 0 && iImportIndex < static_cast<int>(PackageImports.size()));
	}
	return false;
}

std::string CMassEffect3Package::GetName(int aIndex) const
{
	if (aIndex >= 0 && static_cast<size_t>(aIndex) < PackageNames.size())
	{
		return PackageNames[aIndex];
	};

	if (aIndex < 0)
	{
		int iImportIndex = -aIndex - 1;
		if (iImportIndex >= 0 && iImportIndex < static_cast<int>(PackageImports.size()))
		{
			return PackageImports[iImportIndex]->GetObjectName();
		}
	}
	return "";
}

std::vector<std::string> CMassEffect3Package::GetNames() const
{
	return PackageNames;
}

std::vector<ObjectParameters> CMassEffect3Package::GetObjectParameters()
{
	return Parameters;
}

void CMassEffect3Package::SetObjectParameters(std::vector<ObjectParameters> ObjectParameters)
{
	Parameters = ObjectParameters;
}

void CMassEffect3Package::ReadPackageData(const std::vector<uint8_t> &data)
{

	std::cout << "Инфо о пакете:" << std::endl;
	std::cout << "  Общий размер: " << data.size() << std::endl;
	std::cout << "  NameCount: " << GetNameCount() << std::endl;
	std::cout << "  NameOffset: 0x" << std::hex << GetNameOffset() << std::dec << std::endl;
	std::cout << "  ImportCount: " << GetImportCount() << std::endl;
	std::cout << "  ImportOffset: 0x" << std::hex << GetImportOffset() << std::dec << std::endl;
	std::cout << "  ExportCount: " << GetExportCount() << std::endl;
	std::cout << "  ExportOffset: 0x" << std::hex << GetExportOffset() << std::dec << std::endl;
	std::cout << "  Flags: 0x" << std::hex << GetFlags() << std::dec << std::endl;

	// Проверяем, что смещения в пределах данных
	if (static_cast<size_t>(GetNameOffset()) >= data.size())
	{
		std::cerr << "\nПРЕДУПРЕЖДЕНИЕ: NameOffset выходит за пределы данных!" << std::endl;
	};
	if (static_cast<size_t>(GetImportOffset()) >= data.size())
	{
		std::cerr << "\nПРЕДУПРЕЖДЕНИЕ: ImportOffset выходит за пределы данных!" << std::endl;
	};
	if (static_cast<size_t>(GetExportOffset()) >= data.size())
	{
		std::cerr << "\nПРЕДУПРЕЖДЕНИЕ: ExportOffset выходит за пределы данных!" << std::endl;
	};

	// Чтение имен
	ReadNames(data, GetNameOffset(), GetNameCount());
	// Чтение импортов
	ReadImports(data, GetImportOffset(), GetImportCount());
	// Чтение экспортов
	ReadExports(data, GetExportOffset(), GetExportCount());


	std::cout << "\nЧтение имён, импортов и экспортов завершено." << std::endl;
}

void CMassEffect3Package::ReadNames(const std::vector<uint8_t> &data, int offset, int count)
{
	PackageNames.clear();
	ZBlock::BinaryReader BinReader(data);
	BinReader.seek(offset);

	std::cout << "\nЧтение " << count << " имен..." << std::endl;

	for (int i = 0; i < count; i++)
	{
		int32_t strLength = BinReader.readS32();
		std::string str;

		if (strLength < 0)
		{
			str = BinReader.readUnicodeString(-strLength);
		} else {
			str = BinReader.readAsciiString(strLength);
		}
		PackageNames.push_back(str);

		std::cout << "  Имя[" << i << "]: '" << str << "' (длина: " << strLength << ")" << std::endl;
	};
}

void CMassEffect3Package::ReadImports(const std::vector<uint8_t> &data, int offset, int count)
{
	PackageImports.clear();
	ZBlock::BinaryReader BinReader(data);
	BinReader.seek(offset);

	std::cout << "\nЧтение " << count << " импортов..." << std::endl;

	for (int i = 0; i < count; i++)
	{
		/*
				CImportEntry imp(this);
				imp.SetIndex(i);

				std::cout << "  Импорт[" << i << "]: "
							  << "Пакет: " << imp.GetIdxPackageName()
							  << ", Класс: " << imp.GetIdxClassName()
							  << ", Имя: " << imp.GetIdxObjectName() << std::endl;
				*/

		auto imp = std::make_unique<CImportEntry>(this, BinReader);
		imp->SetIndex(i);

		std::cout << "  Импорт[" << i << "]: "
				  << "Пакет: " << imp->GetIdxPackageName()
				  << " (" << GetName(imp->GetIdxPackageName()) << ")"
				  << ", Класс: " << imp->GetIdxClassName()
				  << " (" << GetName(imp->GetIdxClassName()) << ")"
				  << ", Имя: " << imp->GetIdxObjectName()
				  << " (" << GetName(imp->GetIdxObjectName()) << ")"
				  << std::endl;

		PackageImports.push_back(std::move(imp));
	};
}

void CMassEffect3Package::ReadExports(const std::vector<uint8_t> &data, int offset, int count)
{
	PackageExports.clear();
	PackageExports.reserve(count);
	ZBlock::BinaryReader BinReader(data);
	BinReader.seek(static_cast<size_t>(offset));

	std::cout << "\nЧтение " << count << " экспортов из offset 0x"
			  << std::hex << offset << std::dec << std::endl;

	for (int i = 0; i < count && BinReader.remaining() >= 68; i++)
	{

		try
		{
			size_t currentPos = BinReader.tell();
			size_t remainingBefore = BinReader.remaining();

//			std::cout << "  Экспорт[" << i << "] позиция 0x" << std::hex
//					  << currentPos << std::dec << " (осталось: "
//					  << remainingBefore << " байт)" << std::endl;

			auto exp = std::make_unique<CME3ExportEntry>(this, BinReader);
			exp->SetIndex(i);

			std::cout << "  Экспорт[" << i << "]: "
					  << "Класс: " << exp->GetIdxClass()
					  << " (" << exp->GetClassName() << ")"
					  << ", Имя: " << exp->GetIdxObjectName()
					  << " (" << GetName(exp->GetIdxObjectName()) << ")"
					  << ", Размер: " << exp->GetDataSize() << std::endl;

			PackageExports.push_back(std::move(exp));
		}
		catch (...)
		{
			if (BinReader.remaining() >= 68)
			{
				BinReader.seek(BinReader.tell() + 68);
			}
			else
			{
				break;
			};
		};
	};
}

CMassEffect3Package::CMassEffect3Package(const std::string& PCCiFilePath) : PackageFileName(PCCiFilePath)
{
	// Имя файла пакета равно отданному имени
	PackageFileName = PCCiFilePath;
}

std::string CMassEffect3Package::GetPackageName()
{
	// Ищем слэши
	size_t LastSlashPos = PackageFileName.find_last_of("/\\");
	// Если находим, то удаляем их
	if (LastSlashPos != std::string::npos)
	{
		PackageFileName = PackageFileName.substr(LastSlashPos + 1);
	}
	// Возвращаем имя файла без расширения (Например: ./PCC/BioA_MPDish_300.pcc -> BioA_MPDish_300)
	return PackageFileName.substr(0, PackageFileName.length()-4);
}

int CMassEffect3Package::GetPackageHeaderSize() { return PackageHeaderSize; }

void CMassEffect3Package::DebugOutputPackageHeader()
{
	// Открываем файл
	std::ifstream PackageFile(PackageFileName, std::ios::binary);
	// Если не удалось открыть файл, то закрываем программу
	if (!PackageFile)
	{
		std::cerr << "Не удалось открыть файл." << std::endl;
		exit(1);
	};

	PackageFile.read(reinterpret_cast<char*>(&PackageHeader), sizeof(PCCiFileHeader));
	// Вычисляем размер файла
	PackageFile.seekg(0, std::ios::end);
	size_t PackageFileSize = PackageFile.tellg();
	PackageFile.seekg(0, std::ios::beg);

	// Выводим данные заголовка
	std::cout << "Размер файла: >>" << PackageFileSize << "<<\n=====" << std::endl;
	std::cout << "sizeof(PackageHeader): >>" << sizeof(PackageHeader) << "<<\n=====" << std::endl;
	std::cout << "Magic Number [0x9E2A83C1][" << sizeof(PackageHeader.MagicNumber) << "]: >>" << std::hex << PackageHeader.MagicNumber << "<< | В коде: " << MagicNumber << std::endl;
	std::cout << "Version [0xC202AC][" << sizeof(PackageHeader.Version) << "]: >>" << std::hex << PackageHeader.Version << "<<" << std::endl;
	std::cout << "Unknown1 [0x67c][" << sizeof(PackageHeader.Unknown1) << "]: >>" << std::hex << PackageHeader.Unknown1 << "<<" << std::endl;
	std::cout << "Unknown2 [0xFFFFFFFB][" << sizeof(PackageHeader.Unknown2) << "]: >>" << std::hex << PackageHeader.Unknown2 << "<<" << std::endl;
	std::cout << "PackageFlags [0x8088000D = 2156396557d][" << sizeof(PackageHeader.PackageFlags) << "]: >>" << std::hex << PackageHeader.PackageFlags << "<<" << std::endl;
	std::cout << "PackageTypeIndicator [0 = Normal Package, 1 = Patch Package (only for use in Patch_001.sfar)][" << sizeof(PackageHeader.PackageTypeIndicator) << "]: >>" << std::hex << PackageHeader.PackageTypeIndicator << "<<" << std::endl;
	std::cout << "NameCount [0x23 = 25d][" << sizeof(PackageHeader.NameCount) << "]: >>" << std::hex << PackageHeader.NameCount << "<<" << std::endl;
	std::cout << "PointerToNameTable [0x8E = 142d][" << sizeof(PackageHeader.PointerToNameTable) << "]: >>" << std::hex << PackageHeader.PointerToNameTable << "<<" << std::endl;
	std::cout << "ExportCount [0x6 = 6d][" << sizeof(PackageHeader.ExportCount) << "]: >>" << std::hex << PackageHeader.ExportCount << "<<" << std::endl;
	std::cout << "PointerToExportTable [0x4c8 = 1224d][" << sizeof(PackageHeader.PointerToExportTable) << "]: >>" << std::hex << PackageHeader.PointerToExportTable << "<<" << std::endl;
	std::cout << "ImportCount [0x5 = 5d][" << sizeof(PackageHeader.ImportCount) << "]: >>" << std::hex << PackageHeader.ImportCount << "<<" << std::endl;
	std::cout << "PointerToImportTable [0x43C = 1084h][" << sizeof(PackageHeader.PointerToImportTable) << "]: >>" << std::hex << PackageHeader.PointerToImportTable << "<<" << std::endl;
	std::cout << "Unknown3 [0x664 = 1636d][" << sizeof(PackageHeader.Unknown3) << "]: >>" << std::hex << PackageHeader.Unknown3 << "<<" << std::endl;
	std::cout << "Unknown4 [0x67C = 1660h][" << sizeof(PackageHeader.Unknown4) << "]: >>" << std::hex << PackageHeader.Unknown4 << "<<" << std::endl;
	std::cout << "Unknown5 [0x0][" << sizeof(PackageHeader.Unknown5) << "]: >>" << std::hex << PackageHeader.Unknown5 << "<<" << std::endl;
	std::cout << "Unknown6 [0x0][" << sizeof(PackageHeader.Unknown6) << "]: >>" << std::hex << PackageHeader.Unknown6 << "<<" << std::endl;
	std::cout << "Unknown7 [0x0][" << sizeof(PackageHeader.Unknown7) << "]: >>" << std::hex << PackageHeader.Unknown7 << "<<" << std::endl;
	std::cout << "Guid1 [0x1241956A = 306287978d][" << sizeof(PackageHeader.Guid1) << "]: >>" << std::hex << PackageHeader.Guid1 << "<<" << std::endl;
	std::cout << "Guid2 [0x48ECB3A7 = 1223472039d][" << sizeof(PackageHeader.Guid2) << "]: >>" << std::hex << PackageHeader.Guid2 << "<<" << std::endl;
	std::cout << "Guid3 [0x8EDAE68B = 2396710539d][" << sizeof(PackageHeader.Guid3) << "]: >>" << std::hex << PackageHeader.Guid3 << "<<" << std::endl;
	std::cout << "Guid4 [0xB945722C = 3108336172d][" << sizeof(PackageHeader.Guid4) << "]: >>" << std::hex << PackageHeader.Guid4 << "<<" << std::endl;
	std::cout << "Generations [0x1][" << sizeof(PackageHeader.Generations) << "]: >>" << std::hex << PackageHeader.Generations << "<<" << std::endl;
	std::cout << "ExportCount2 [0x6 = 6d][" << sizeof(PackageHeader.ExportCount2) << "]: >>" << std::hex << PackageHeader.ExportCount2 << "<<" << std::endl;
	std::cout << "NameCount2 [0x23 = 35d][" << sizeof(PackageHeader.NameCount2) << "]: >>" << std::hex << PackageHeader.NameCount2 << "<<" << std::endl;
	std::cout << "Unknown8 [0x0 = 0d][" << sizeof(PackageHeader.Unknown8) << "]: >>" << std::hex << PackageHeader.Unknown8 << "<<" << std::endl;
	std::cout << "Engine [0x18EF = 6383d][" << sizeof(PackageHeader.Engine) << "]: >>" << std::hex << PackageHeader.Engine << "<<" << std::endl;
	std::cout << "Unknown9 [0x3006B = 196715d][" << sizeof(PackageHeader.Unknown9) << "]: >>" << std::hex << PackageHeader.Unknown9 << "<<" << std::endl;
	std::cout << "Unknown10 [0x15330000 = 355663872d][" << sizeof(PackageHeader.Unknown10) << "]: >>" << std::hex << PackageHeader.Unknown10 << "<<" << std::endl;
	std::cout << "Unknown11 [0x8AA0000 = 145358848d][" << sizeof(PackageHeader.Unknown11) << "]: >>" << std::hex << PackageHeader.Unknown11 << "<<" << std::endl;
	std::cout << "CompressionType [0 = None, 1 = Zlib, 2 = LZO. Unused if Package Flags does not set Compressed bit.][" << sizeof(PackageHeader.CompressionType) << "]: >>" << std::hex << PackageHeader.CompressionType << "<<" << std::endl;
	std::cout << "ChunkCount [0x0 или 0x1][" << sizeof(PackageHeader.ChunkCount) << "]: >>" << std::hex << PackageHeader.ChunkCount << "<<" << std::endl;
	std::cout << "=====" << std::endl;
	std::cout << "sizeof(PackageHeader): >>" << sizeof(PackageHeader) << "<<" << std::endl;
	std::cout << "=====" << std::endl;

	// Закрываем файл
	PackageFile.close();
}

void CMassEffect3Package::PackageFileInfo()
{
	std::ifstream PackageFile(PackageFileName, std::ios::binary);

	if (!PackageFile)
	{
		std::cerr << "\nНе удалось открыть файл." << std::endl;
		exit(1);
	};

	PackageFile.read(reinterpret_cast<char*>(&PackageHeader), sizeof(PCCiFileHeader));
	PackageFile.seekg(0, std::ios::end);
	//size_t PackageFileSize = PackageFile.tellg();
	PackageFile.seekg(0, std::ios::beg);

	if (MagicNumber != PackageHeader.MagicNumber)
	{
		throw std::runtime_error("\nЭто не pcc-файл");
	};

	PackageFile.close();

	std::cout << "\n==== " << GetPackageName() << " =====" << std::endl;
	std::cout << "Количество имён: " << GetNameCount() << std::endl;
	std::cout << "Количество импортов: " << GetImportCount() << std::endl;
	std::cout << "Количество экспортов: " << GetExportCount() << std::endl;
	std::cout << "\n" << std::endl;
}

void CMassEffect3Package::ToRead()
{
	DebugOutputPackageHeader();

	std::ifstream PackageFile(PackageFileName, std::ios::binary);

	if (!PackageFile)
	{
		std::cerr << "\nНе удалось открыть файл." << std::endl;
		exit(1);
	};

	PackageFile.read(reinterpret_cast<char*>(&PackageHeader), sizeof(PCCiFileHeader));
	PackageFile.seekg(0, std::ios::end);
	//size_t PackageFileSize = PackageFile.tellg();
	PackageFile.seekg(0, std::ios::beg);

	if (MagicNumber != PackageHeader.MagicNumber)
	{
		throw std::runtime_error("\nЭто не pcc-файл");
	};

	// Разжатие pcc-файла
	std::vector<uint8_t> DecompressedPackageFile = DecompressME3(PackageFile);
	PackageFile.close();

	ReadPackageData(DecompressedPackageFile);

	std::vector<uint8_t>().swap(DecompressedPackageFile);
	std::cout << "\nРазжатости очищены." << std::endl;

	Parameters = ExtractObjectsFromLevel(PackageNames, PackageImports, PackageExports, GetPackageName());
}
