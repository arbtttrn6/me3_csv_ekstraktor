#ifndef CIMEPACKAGE_H
#define CIMEPACKAGE_H

#include <memory>
#include <string>
#include <vector>
#include "CIEntryProvider.h"

/*--------------------------------------------------------------------
CIMEPackage -- класс пакетов Mass Effect
--------------------------------------------------------------------*/
class CIMEPackage : public CIEntryProvider
{
//private:
	//std::vector<std::unique_ptr<CIEntry>> PackageExports;
	//std::vector<std::unique_ptr<CIEntry>> PackageImports;
	//std::vector<std::string> PackageNames;

//public:
	//virtual ~CIMEPackage() = default;

	//virtual bool IsCompressed() const = 0;
	//virtual int GetExportCount() const = 0;
	//virtual int GetImportCount() const = 0;
	//virtual const std::vector<std::string>& GetNames() const = 0;
	//virtual const std::vector<std::unique_ptr<CExportEntry>>& GetExports() const = 0;
	//virtual const std::vector<std::unique_ptr<CImportEntry>>& GetImports() const = 0;

	//virtual std::string GetNameEntry(int index) const = 0;
	//virtual std::unique_ptr<CExportEntry> GetExport(int index) const = 0;
	//virtual std::unique_ptr<CImportEntry> GetImport(int index) const = 0;

	//virtual CExportEntry* GetEntry(int32_t aIndex) const = 0;
	//virtual std::string GetName(int aIndex) const = 0;
	//virtual bool IsName(int aIndex) const = 0;

//	CIEntry* GetEntry(int aIndex) const override;;

//	std::string GetName(int aIndex) const override;

//	bool IsName(int aIndex) const override;

};

#endif // CIMEPACKAGE_H
