#ifndef CEXPORTENTRY_H
#define CEXPORTENTRY_H

#include "CIEntry.h"

//#include "propertytypedef.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
//#include "cimepackage.h"
#include "Propertydef.h"


/*--------------------------------------------------------------------
CExportEntry -- класс экспортов
--------------------------------------------------------------------*/
class CExportEntry : public CIEntry
{
protected:
		CIEntryProvider* Provider;

		std::vector<uint8_t> Header;
		std::vector<uint8_t> Data;
		int Index;
		uint32_t HeaderOffset;
		int OriginalDataSize;
		bool DataChanged;
		bool HeaderChanged;
		bool ReadsFromConfig;

private:
		std::vector<Property> m_Properties;
		int iPropsEndOffset;

public:
		CExportEntry(CIEntryProvider* aProvider);;

		int GetIndex() const override { return Index; }
		void SetIndex(int value) { Index = value; }
		int GetUIndex() const override { return Index + 1; }

		const std::vector<uint8_t>& GetHeader() const;
		void SetHeader(const std::vector<uint8_t>& newHead);;

		int GetPropsEndOffset();

		uint32_t GetHeaderOffset() const;
		void SetHeaderOffset(uint32_t value);

		int GetIdxClass() const;
		void SetIdxClass(int value);

		int GetIdxClassParent() const;
		void SetIdxClassParent(int value);

		int GetIdxLink() const override;
		void SetIdxLink(int value);

		int GetIdxObjectName() const override;
		void SetIdxObjectName(int value);

		int GetIdxPackageName() const ;

		int GetIndexValue() const;
		void SetIndexValue(int value);

		int GetIdxArchtype() const;
		void SetIdxArchtype(int value);

		uint64_t GetObjectFlags() const;
		void SetObjectFlags(uint64_t value);

		int GetDataSize() const;
		void SetDataSize(int value);

		int GetDataOffset() const;
		void SetDataOffset(int value);

		std::string GetObjectName() const override;
		std::string GetClassName() const override;;

		std::string GetSuperClass() const;

		std::string GetClassParent() const;
		std::string GetArchtypeName() const;
		std::string GetPackageName() const override;
		std::string GetPackageFullName() const override;
		std::string GetFullPath() const override;

		const std::vector<uint8_t>& GetData() const;;
		void SetData(const std::vector<uint8_t>& value);;

		int GetOriginalDataSize() const;;
		bool GetReadsFromConfig() const;;

		bool IsDataChanged() const;;
		void SetDataChanged(bool value);;

		bool IsHeaderChanged() const;;
		void SetHeaderChanged(bool value);;

		bool HasStack() const;
		int GetPropertyStart() const;;

		std::vector<Property> ReadProperties(const std::vector<uint8_t>& aData, int iStart);;
		Property ReadProperty(const std::vector<uint8_t>& aData, int& iPos);;

		//PropertyType GetType(std::string sTypeName);

		std::vector<Property> GetProperties();;
		std::vector<uint8_t> GetBinaryData() const;;

		void SetBinaryData(const std::vector<uint8_t>& BinaryData);;

		std::string GetPackageNameWithoutPostfix() const;

		CIEntryProvider* GetProvider();
private:
		int32_t GetInt32FromHeader(size_t offset) const;;
		void SetInt32ToHeader(size_t offset, int32_t value);;

		uint64_t GetUInt64FromHeader(size_t offset) const;;
		void SetUInt64ToHeader(size_t offset, uint64_t value);;

		int32_t GetInt32FromData(size_t offset) const;;
		int32_t GetInt32FromOtherData(std::vector<uint8_t> aData, size_t aOffset) const;;
};

#endif // CEXPORTENTRY_H
