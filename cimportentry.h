#ifndef CIMPORTENTRY_H
#define CIMPORTENTRY_H

#pragma once

//#include "cimepackage.h"
#include "zblock.h"
#include "CIEntry.h"

/*--------------------------------------------------------------------
CImportEntry -- класс импортов
--------------------------------------------------------------------*/
class CImportEntry : public CIEntry
{
private:
		CIEntryProvider* Provider;

		// Заголовок
		std::vector<uint8_t> Header;
		// Индекс
		int Index;
		// Изменён ли заголовок
		bool HeaderChanged;
public:
	static const int ByteSize = 28;

	CImportEntry(CIEntryProvider* aProvider, ZBlock::BinaryReader& ImportData);;

	CImportEntry(CIEntryProvider* aProvider);;

	int GetIndex() const override;;
	void SetIndex(int value);;

	int GetUIndex() const override;;

	int GetIdxClass() const;

	const std::vector<uint8_t>& GetHeader() const;;
	void SetHeader(const std::vector<uint8_t>& NewHeader);;

	int GetIdxPackageName() const;;
	void SetIdxPackageName(int value);;

	int GetIdxClassName() const;;
	void SetIdxClassName(int value);;

	int GetIdxLink() const override;;
	void SetIdxLink(int value);;

	int GetIdxObjectName() const override;;
	void SetIdxObjectName(int value);;

	std::string GetClassName() const override;
	std::string GetPackageFile() const;
	std::string GetObjectName() const override;
	std::string GetPackageName() const override;
	std::string GetPackageFullName() const override;
	std::string GetFullPath() const override;

	bool IsHeaderChanged() const;;
	void SetHeaderChanged(bool value);;

	CImportEntry clone() const;;

private:
	int32_t GetInt32FromHeader(size_t offset) const;;

	void SetInt32ToHeader(size_t offset, int32_t value);;
};

#endif // CIMPORTENTRY_H
