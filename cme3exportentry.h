#ifndef CME3EXPORTENTRY_H
#define CME3EXPORTENTRY_H

#include "cexportentry.h"
#include "zblock.h"
#include <iostream>

/*--------------------------------------------------------------------
ME3ExportEntry -- класс экспортов Mass Effect 3
--------------------------------------------------------------------*/
class CME3ExportEntry : public CExportEntry
{
public:
	CME3ExportEntry(CIEntryProvider* aProvider, ZBlock::BinaryReader& stream);;

	CME3ExportEntry(CIEntryProvider* aProvider) : CExportEntry(aProvider) {};
};

#endif // CME3EXPORTENTRY_H
