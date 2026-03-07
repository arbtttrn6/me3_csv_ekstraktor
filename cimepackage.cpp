#include "cimepackage.h"
#include <iostream>

/*--------------------------------------------------------------------
CIMEPackage -- класс пакетов Mass Effect
--------------------------------------------------------------------*/

//CIEntry* CIMEPackage::GetEntry(int aIndex) const {
//	if (aIndex > 0) {
//		size_t ExportIndex = static_cast<size_t>(aIndex - 1);

//		if (ExportIndex >= PackageExports.size()) {
//			std::cerr << "\nОшибка: вызов GetEntry с неверным индексом экспорта: " << aIndex
//					  << ". Максимальный индекс экспорта: " << PackageExports.size() - 1 << std::endl;
//			return nullptr;
//		};

//		//return PackageExports[aIndex - 1].get();
//		return PackageExports[ExportIndex].get();
//	} else if (aIndex < 0) {
//		size_t ImportIndex = static_cast<size_t>(-(aIndex + 1));

//		if (ImportIndex >= PackageImports.size()) {
//			std::cerr << "\nОшибка: вызов GetEntry с неверным индексом импорта: " << aIndex
//					  << ". Максимальный индекс импорта: " << PackageImports.size() - 1 << std::endl;
//			return nullptr;
//		};

//		return PackageImports[ImportIndex].get();
//	}

//	return nullptr;

//	//return PackageImports[-aIndex - 1].get();
//}


//std::string CIMEPackage::GetName(int aIndex) const {
//	return Names[aIndex];
//}

//std::string CIMEPackage::GetName(int aIndex) const {
//	if (aIndex >= 0 && static_cast<size_t>(aIndex) < PackageNames.size()) {
//		return PackageNames[aIndex];
//	};
//	return "";
//}

//bool CIMEPackage::IsName(int aIndex) const {
//	return (aIndex > 0);
//}
