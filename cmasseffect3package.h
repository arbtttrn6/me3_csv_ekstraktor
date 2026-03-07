#ifndef CMASSEFFECT3PACKAGE_H
#define CMASSEFFECT3PACKAGE_H

#include "cimepackage.h"
#include "cimportentry.h"
#include "cexportentry.h"
#include "objectdef.h"
#include "pccdef.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*--------------------------------------------------------------------
MassEffect3Package -- класс пакета Mass Effect 3
--------------------------------------------------------------------*/
class CMassEffect3Package : public CIMEPackage
{
private:
	// Имя файла пакета (Например: BioA_MPDish_200.pcc)
	std::string PackageFileName;
	// Заголовок пакета
	PCCiFileHeader PackageHeader;
	// Имена пакета
	std::vector<std::string> PackageNames;
	// Импорты пакета
	std::vector<std::unique_ptr<CImportEntry>> PackageImports;
	// Экспорты пакета
	std::vector<std::unique_ptr<CExportEntry>> PackageExports;
	// Размер заголовка
	static const uint32_t PackageHeaderSize = 0x8E;
	// Magic Number
	static const uint32_t MagicNumber = 0x9E2A83C1;

	std::string wideStringToUTF8(const std::wstring& wideStr);;

	std::vector<ObjectParameters> Parameters;

public:
	uint32_t GetFlags() const;;

	int GetNameCount() const;;
	int GetNameOffset() const;;

	int GetExportCount() const;;
	int GetImportCount() const;;

	int GetImportOffset() const;;
	int GetExportOffset() const;;

	bool bIsExport(int32_t aIndex);;

	CExportEntry* GetExports(int32_t aIndex);;
	CImportEntry* GetImports(int32_t aIndex);;

	CIEntry* GetEntry(int aIndex) const override;;

	bool IsName(int aIndex) const override;;

	std::string GetName(int aIndex) const override;;

	std::vector<std::string> GetNames() const;

	std::vector<ObjectParameters> GetObjectParameters();
	void SetObjectParameters(std::vector<ObjectParameters> ObjectParameters);

	void ReadPackageData(const std::vector<uint8_t>& data);;

	void ReadNames(const std::vector<uint8_t>& data, int offset, int count);;
	void ReadImports(const std::vector<uint8_t>& data, int offset, int count);;
	void ReadExports(const std::vector<uint8_t>& data, int offset, int count);;

	CMassEffect3Package(const std::string& PCCiFilePath);;

	// Функция получения имени файла (без пути и расширения файла)
	std::string GetPackageName();;

	// Функция получения размера заголовка пакета
	int GetPackageHeaderSize();;

	// Отладочная функция
	void DebugOutputPackageHeader();
	// Справочная функция
	void PackageFileInfo();

	void ToRead();
};

#endif // CMASSEFFECT3PACKAGE_H
