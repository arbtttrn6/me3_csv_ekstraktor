#include "cimportentry.h"
#include "cimepackage.h"
#include <iostream>


/*--------------------------------------------------------------------
CImportEntry -- класс импортов
--------------------------------------------------------------------*/
CImportEntry::CImportEntry(CIEntryProvider* aProvider, ZBlock::BinaryReader &ImportData) :
	Provider(aProvider), Index(0), HeaderChanged(false)
{

	Header.resize(ByteSize);
	ImportData.readBytes(Header.data(), ByteSize);
}

CImportEntry::CImportEntry(CIEntryProvider* aProvider) :
	Provider(aProvider), Index(0), HeaderChanged(false)
{

	Header.resize(ByteSize);
}

int CImportEntry::GetIndex() const { return Index; }

void CImportEntry::SetIndex(int value) { Index = value; }

int CImportEntry::GetUIndex() const { return -Index - 1; }

int CImportEntry::GetIdxClass() const { return GetInt32FromHeader(0); }

const std::vector<uint8_t> &CImportEntry::GetHeader() const { return Header; }

void CImportEntry::SetHeader(const std::vector<uint8_t> &NewHeader)
{
	Header = NewHeader;
	HeaderChanged = true;
}

int CImportEntry::GetIdxPackageName() const { return GetInt32FromHeader(0); }

void CImportEntry::SetIdxPackageName(int value)
{
	SetInt32ToHeader(0, value);
	HeaderChanged = true;
}

int CImportEntry::GetIdxClassName() const { return GetInt32FromHeader(8); }

void CImportEntry::SetIdxClassName(int value)
{
	SetInt32ToHeader(8, value);
	HeaderChanged = true;
}

int CImportEntry::GetIdxLink() const { return GetInt32FromHeader(16); }

void CImportEntry::SetIdxLink(int value)
{
	SetInt32ToHeader(16, value);
	HeaderChanged = true;
}

int CImportEntry::GetIdxObjectName() const { return GetInt32FromHeader(20); }

void CImportEntry::SetIdxObjectName(int value)
{
	SetInt32ToHeader(20, value);
	HeaderChanged = true;
}

std::string CImportEntry::GetClassName() const
{
//	int32_t Value = GetIdxClass();

//	if (Value != 0) {
//		CIEntry* ClassEntry = Provider->GetEntry(Value);
//		return ClassEntry->GetObjectName();
//	};

//	return "Class";
	return Provider->GetName(GetIdxClassName());

}

std::string CImportEntry::GetObjectName() const
{
	if (!Provider)
	{
		std::cerr << "CImportEntry::GetObjectName: не существует!" << std::endl;
	}
	return Provider->GetName(GetIdxObjectName());
}

std::string CImportEntry::GetPackageName() const
{
	return Provider->GetName(GetIdxPackageName()) + ".pcc";
}

std::string CImportEntry::GetPackageFullName() const
{
	std::string sResult = GetPackageName();
	int iDxNewPackageName = GetIdxLink();

	while (iDxNewPackageName != 0)
	{
		CIEntry* NewEntry = Provider->GetEntry(iDxNewPackageName);
		std::string sNewPackageName = NewEntry->GetPackageName();

		if (sNewPackageName != "Package")
		{
			sResult = sNewPackageName + "." + sResult;
		}

		iDxNewPackageName = NewEntry->GetIdxLink();
	}
	return sResult;
}

std::string CImportEntry::GetFullPath() const
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

bool CImportEntry::IsHeaderChanged() const { return HeaderChanged; }

void CImportEntry::SetHeaderChanged(bool value) { HeaderChanged = value; }

CImportEntry CImportEntry::clone() const
{
	CImportEntry NewImport(Provider);
	NewImport.Header = Header;
	NewImport.Index = Index;
	NewImport.HeaderChanged = HeaderChanged;

	return NewImport;
}

int32_t CImportEntry::GetInt32FromHeader(size_t offset) const
{
	if (offset + 4 > Header.size()) { return 0; };
	int32_t value;
	memcpy(&value, Header.data() + offset, 4);

	return value;
}

void CImportEntry::SetInt32ToHeader(size_t offset, int32_t value)
{
	if (offset + 4 > Header.size()) { return; };
	memcpy(Header.data() + offset, &value, 4);
}
