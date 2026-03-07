#ifndef CIENTRYPROVIDER_H
#define CIENTRYPROVIDER_H

#include "CIEntry.h"
#include <string>


/*--------------------------------------------------------------------
CIEntryProvider -- класс-проводник для класса вхождений
--------------------------------------------------------------------*/
class CIEntryProvider
{
public:
	virtual ~CIEntryProvider() = default;
	virtual std::string GetName(int aIndex) const = 0;
	//	virtual std::string GetPackageName() const = 0;
	virtual CIEntry* GetEntry(int aIndex) const = 0;
	virtual bool IsName(int aIndex) const = 0;
	//virtual int FindName(const std::string& sName) const = 0;
};

#endif // CIENTRYPROVIDER_H
