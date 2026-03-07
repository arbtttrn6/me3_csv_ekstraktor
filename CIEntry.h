#ifndef CIENTRY_H
#define CIENTRY_H

#include <string>

class CIEntryProvider;

/*--------------------------------------------------------------------
CIEntry -- класс вхождений
--------------------------------------------------------------------*/
class CIEntry
{
public:
	virtual ~CIEntry() = default;
	virtual int GetIndex() const = 0;
	virtual int GetUIndex() const = 0;
	virtual int GetIdxLink() const = 0;
	virtual int GetIdxObjectName() const = 0;
	virtual std::string GetObjectName() const = 0;
	virtual std::string GetClassName() const = 0;
	virtual std::string GetPackageName() const = 0;
	virtual std::string GetPackageFullName() const = 0;
	virtual std::string GetFullPath() const = 0;

protected:
	CIEntryProvider* Provider;
};

#endif // CIENTRY_H
