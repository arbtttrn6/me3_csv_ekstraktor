#include "cexportentry.h"
#include "cimepackage.h"
#include "propertytypedef.h"
#include <iostream>

#include "utils.h"

/*--------------------------------------------------------------------
CExportEntry -- класс экспортов
--------------------------------------------------------------------*/

CExportEntry::CExportEntry(CIEntryProvider* aProvider) : Provider(aProvider), Index(0), HeaderOffset(0),
											OriginalDataSize(0), DataChanged(false),
											HeaderChanged(false), ReadsFromConfig(false) {}

const std::vector<uint8_t> &CExportEntry::GetHeader() const { return Header; }

void CExportEntry::SetHeader(const std::vector<uint8_t> &newHead)
{
	Header = newHead;
	HeaderChanged = true;
}

int CExportEntry::GetPropsEndOffset() { return iPropsEndOffset; }

uint32_t CExportEntry::GetHeaderOffset() const { return HeaderOffset; }

void CExportEntry::SetHeaderOffset(uint32_t value) { HeaderOffset = value; }

int CExportEntry::GetIdxClass() const { return GetInt32FromHeader(0); }

void CExportEntry::SetIdxClass(int value) { SetInt32ToHeader(0, value); HeaderChanged = true; }

int CExportEntry::GetIdxClassParent() const { return GetInt32FromHeader(4); }

void CExportEntry::SetIdxClassParent(int value) { SetInt32ToHeader(4, value); HeaderChanged = true; }

int CExportEntry::GetIdxLink() const { return GetInt32FromHeader(8); }

void CExportEntry::SetIdxLink(int value) { SetInt32ToHeader(8, value); HeaderChanged = true; }

int CExportEntry::GetIdxObjectName() const { return GetInt32FromHeader(12); }

void CExportEntry::SetIdxObjectName(int value) { SetInt32ToHeader(12, value); HeaderChanged = true; }

int CExportEntry::GetIdxPackageName() const { return GetInt32FromHeader(20); }

int CExportEntry::GetIndexValue() const { return GetInt32FromHeader(16); }

void CExportEntry::SetIndexValue(int value) { SetInt32ToHeader(16, value); HeaderChanged = true; }

int CExportEntry::GetIdxArchtype() const { return GetInt32FromHeader(20); }

void CExportEntry::SetIdxArchtype(int value) { SetInt32ToHeader(20, value); HeaderChanged = true; }

uint64_t CExportEntry::GetObjectFlags() const { return GetUInt64FromHeader(24); }

void CExportEntry::SetObjectFlags(uint64_t value) { SetUInt64ToHeader(24, value); HeaderChanged = true; }

int CExportEntry::GetDataSize() const { return GetInt32FromHeader(32); }

void CExportEntry::SetDataSize(int value) { SetInt32ToHeader(32, value); }

int CExportEntry::GetDataOffset() const { return GetInt32FromHeader(36); }

void CExportEntry::SetDataOffset(int value) { SetInt32ToHeader(36, value); }

std::string CExportEntry::GetObjectName() const { return Provider->GetName(GetIdxObjectName()); }

std::string CExportEntry::GetClassName() const
{
	int32_t iValue = GetIdxClass();
	if (iValue != 0)
	{

		if (!Provider)
		{
			std::cerr << "\nОшибка: Provider не ествует!" << std::endl;
			return "Provider не ествует.";
		};

		CIEntry* ClassEntry = Provider->GetEntry(iValue);

		if (!ClassEntry)
		{
			std::cerr << "\nОшибка: ClassEntry по индексу " << iValue << " не существует!" << std::endl;
			return "ClassEntry не ествует.";
		};

		int32_t iObjectNameIndex = ClassEntry->GetIdxObjectName();
		return Provider->GetName(iObjectNameIndex);
	};

	return "Class";
}

std::string CExportEntry::GetSuperClass() const
{
	int32_t iValue = GetIdxClassParent();
	if (iValue != 0)
	{
		CIEntry* SuperClassEntry = Provider->GetEntry(iValue);
		if (SuperClassEntry)
		{
			int32_t iObjectNameIndex = SuperClassEntry->GetIdxObjectName();
			return Provider->GetName(iObjectNameIndex);
		};
	};
	return "Class";
}

std::string CExportEntry::GetArchtypeName() const
{
	int32_t iValue = GetIdxArchtype();
	if (iValue != 0)
	{
		CIEntry* ArchetypeEntry = Provider->GetEntry(iValue);
		if (ArchetypeEntry)
		{
			int32_t iObjectNameIndex = ArchetypeEntry->GetIdxObjectName();
			return Provider->GetName(iObjectNameIndex);
		};
	};
	return "None";
}

std::string CExportEntry::GetPackageName() const
{
	int32_t iValue = GetIdxLink();

	if (iValue != 0)
	{
		CIEntry* NewEntry = Provider->GetEntry(iValue);
		if (NewEntry)
		{
			return Provider->GetName(NewEntry->GetIdxObjectName());
		};
	};
	return "Package";
}

std::string CExportEntry::GetPackageFullName() const
{
	std::string sResult = GetPackageName();
	int iDxNewPackageName = GetIdxLink();

	while (iDxNewPackageName != 0)
	{
		CIEntry* NewEntry = Provider->GetEntry(iDxNewPackageName);
		if (NewEntry)
		{
			std::string sNewPackageName = NewEntry->GetPackageName();

			if (sNewPackageName != "Package")
			{
				sResult = sNewPackageName + "." + sResult;
			}

			iDxNewPackageName = NewEntry->GetIdxLink();
		}
		else
		{
			break;
		}
	}
	return sResult;
}

std::string CExportEntry::GetPackageNameWithoutPostfix() const
{
	return Provider->GetName(GetIdxPackageName());
	//	return Provider->GetPackageName();
}

CIEntryProvider *CExportEntry::GetProvider()
{
	return Provider;
}

std::string CExportEntry::GetFullPath() const
{
	std::string sFullPath = "";
	std::string sPackageName = GetPackageFullName();

	if (sPackageName != "Class" && sPackageName != "Package")
	{
		sFullPath += sPackageName + ".";
	};

	sFullPath += GetObjectName();
	return sFullPath;
}

const std::vector<uint8_t> &CExportEntry::GetData() const { return Data; }

void CExportEntry::SetData(const std::vector<uint8_t> &value)
{
	Data = value;
	SetDataSize(value.size());
	DataChanged = true;
	// сбросить свойства
}

int CExportEntry::GetOriginalDataSize() const { return OriginalDataSize; }

bool CExportEntry::GetReadsFromConfig() const { return ReadsFromConfig; }

bool CExportEntry::IsDataChanged() const { return DataChanged; }

void CExportEntry::SetDataChanged(bool value) { DataChanged = value; }

bool CExportEntry::IsHeaderChanged() const { return HeaderChanged; }

void CExportEntry::SetHeaderChanged(bool value) { HeaderChanged = value; }

bool CExportEntry::HasStack() const
{
	const uint64_t HAS_STACK_FLAG = 0x0200000000000000ULL;
	return (GetObjectFlags() & HAS_STACK_FLAG) != 0;
}

int CExportEntry::GetPropertyStart() const
{
	if (HasStack()) {
		printf("PropertyStart: 30");
		return 30;
	};

	int test1 = GetInt32FromData(4);
	int test2 = GetInt32FromData(8);

	if (Provider->IsName(test1) && test2 == 0)
	{
		printf("\nCExportEntry::GetPropertyStart(): (Provider->IsName(test1) && test2 == 0): PropertyStart: 4\n");
		return 4;
	};
	if (Provider->IsName(test1) && Provider->IsName(test2) && test2 != 0)
	{
		printf("\nCExportEntry::GetPropertyStart(): (Provider->IsName(test1) && Provider->IsName(test2) && test2 != 0): PropertyStart: 8\n");
		return 8;
	};

	printf("\nCExportEntry::GetPropertyStart(): PropertyStart: 8\n");
	return 8;
}

std::vector<Property> CExportEntry::ReadProperties(const std::vector<uint8_t> &aData, int iStart)
{

	std::vector<Property> Result;
	int iPos = iStart;

	while (iPos + 8 <= static_cast<int>(aData.size()))
	{
		int32_t iNameIdx = GetInt32FromOtherData(aData, iPos);

		if (!Provider->IsName(iNameIdx))
		{
			std::cerr << "Неверный iNameIdx: "
					  << iNameIdx << " на позиции "
					  << iPos << std::endl;
			break;
		};

		std::string sName = Provider->GetName(iNameIdx);


		if (strcmp(sName.c_str(), "") == 0)
		{
			break;
		};


		if (strcmp(sName.c_str(), "None") == 0)
		{
			iPropsEndOffset = iPos + 8;
			std::cout << "\nПараметр None найден" << std::endl;

			Property Prop;
			Prop.sName = sName;
			Prop.sType = GetType("None");
			Prop.Data.clear();

			Result.push_back(Prop);

			std::cout << "ReadProperties: 0x" << DecimalToHex(iPos) << ": " << sName << " (None)" << std::endl;

			iPos += 8;

			break;
		};

		if (iPos + 16 >= static_cast<int>(aData.size())) //12
		{
			break;
		}

		int32_t iTypeIdx = GetInt32FromOtherData(aData, iPos + 8);
		if (!Provider->IsName(iTypeIdx))
		{
			break;
		};
		std::string sType = Provider->GetName(iTypeIdx);

		int32_t iSize = GetInt32FromOtherData(aData, iPos + 16);

		if (iSize < 0 || iPos + 24 + iSize > static_cast<int>(aData.size()))
		{
			std::cerr << "\nНеверный размер свойства " << sName
					  << " (" << sType << "): " << DecimalToHex(iSize)
					  << " [" << iSize << "] " << " на позиции "
					  << DecimalToHex(iPos) << " [" << iPos << "]\n" << std::endl;
			break;
		}

		Property Prop;
		Prop.sName = sName;
		Prop.sType = GetType(sType);

		int iDataStart = iPos;

		if (strcmp(sType.c_str(), "StructProperty") == 0) //костыль на костыле
		{
			if (strcmp(sName.c_str(), "location") == 0 ||
				strcmp(sName.c_str(), "Rotation") == 0 ||
				strcmp(sName.c_str(), "DrawScale3D") == 0 ||
				strcmp(sName.c_str(), "LightingChannels") == 0 ||
				strcmp(sName.c_str(), "Scale3D") == 0)
			{
				iDataStart = iPos + 32;
			}
		}
		else
		{
			iDataStart = iPos + 24;
		};
		Prop.Data.assign(
					aData.begin() + iDataStart,
					aData.begin() + iDataStart + iSize
					);

		Result.push_back(Prop);

		std::cout << "ReadProperties: 0x" << DecimalToHex(iDataStart) << ": " << sName << " (" << sType << ")" << std::endl;

		iPos = iDataStart + iSize;

		if (strcmp(sType.c_str(), "StructProperty") == 0 && // ещё один костыль
			strcmp(sName.c_str(), "location") != 0 &&
			strcmp(sName.c_str(), "Rotation") != 0 &&
			strcmp(sName.c_str(), "DrawScale3D") != 0 &&
			strcmp(sName.c_str(), "LightingChannels") != 0 &&
			strcmp(sName.c_str(), "Scale3D") != 0) {
			iPos += 8;
		} else if (strcmp(sType.c_str(), "BoolProperty") == 0) {
			iPos += 1;
		} /*else if (strcmp(sType.c_str(), "ByteProperty") == 0) {
			iPos += 8;
		}*/ /*else if (strcmp(sType.c_str(), "FloatProperty") == 0) {
			iPos += 4;
		}*/ /*else if (strcmp(sType.c_str(), "IntProperty") == 0) {
			iPos += 4;
		}*/
	};

	return Result;

}

Property CExportEntry::ReadProperty(const std::vector<uint8_t> &aData, int &iPos)
{
	Property Prop;

	int32_t iNameIdx = GetInt32FromOtherData(aData, iPos);
	Prop.sName = Provider->GetName(iNameIdx);

	int32_t iTypeIdx = GetInt32FromOtherData(aData, iPos + 8);

	std::string sType = Provider->GetName(iTypeIdx);

	Prop.sType = GetType(sType);

	int32_t iSize = GetInt32FromOtherData(aData, iPos + 16);

	int iDataStart = iPos + 24;
	Prop.Data.assign(
					aData.begin() + iDataStart,
					aData.begin() + iDataStart + iSize
					);

	iPos = iDataStart + iSize;

	return Prop;
}

std::vector<Property> CExportEntry::GetProperties()
{
	if (!m_Properties.empty()) {
		return m_Properties;
	};

	int iStart = GetPropertyStart();

//	std::cout << "\nПервые байты данных: ";
//	for (int i = 0; i < std::min(32, (int)Data.size()); i++)
//	{
//		printf("%02X ", Data[i]);
//	}
//	for (int i = 0; i < static_cast<int>(Data.size()); i++)
//	{
//		printf("%02X ", Data[i]);
//	}

	std::cout << std::endl;

	m_Properties = ReadProperties(Data, iStart);
	std::cout << "\nПрочитано свойств: " << m_Properties.size() << std::endl;

	return m_Properties;
}

std::vector<uint8_t> CExportEntry::GetBinaryData() const
{
	int PropEnd = GetPropertyStart();
	if (static_cast<size_t>(PropEnd) >= Data.size()) return {};

	return std::vector<uint8_t>(Data.begin() + PropEnd, Data.end());
}

void CExportEntry::SetBinaryData(const std::vector<uint8_t> &BinaryData)
{
	int PropEnd = GetPropertyStart();
	if (static_cast<size_t>(PropEnd) > Data.size()) return;

	std::vector<uint8_t> NewData(Data.begin(), Data.begin() + PropEnd);
	NewData.insert(NewData.end(), BinaryData.begin(), BinaryData.end());
	SetData(NewData);
}

int32_t CExportEntry::GetInt32FromHeader(size_t offset) const
{
	if (offset + 4 > Header.size()) return 0;
	int32_t value;
	memcpy(&value, Header.data() + offset, 4);

	return value;
}

void CExportEntry::SetInt32ToHeader(size_t offset, int32_t value)
{
	if (offset + 4 > Header.size()) return;
	memcpy(Header.data() + offset, &value, 4);
}

uint64_t CExportEntry::GetUInt64FromHeader(size_t offset) const
{
	if (offset + 8 > Header.size()) return 0;
	uint64_t value;
	memcpy(&value, Header.data() + offset, 8);

	return value;
}

void CExportEntry::SetUInt64ToHeader(size_t offset, uint64_t value)
{
	if (offset + 8 > Header.size()) return;
	memcpy(Header.data() + offset, &value, 8);
}

int32_t CExportEntry::GetInt32FromData(size_t offset) const
{
	if (offset + 4 > Data.size()) return 0;
	int32_t value;
	memcpy(&value, Data.data() + offset, 4);

	return value;
}

int32_t CExportEntry::GetInt32FromOtherData(std::vector<uint8_t> aData, size_t aOffset) const
{
	if (aOffset + 4 > aData.size()) return 0;
	int32_t iValue;
	memcpy(&iValue, aData.data() + aOffset, 4);

	return iValue;
}
