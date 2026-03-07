#include "cexportentry.h"
#include "cmasseffect3package.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>

#pragma once

/*--------------------------------------------------------------------
PrintBytes -- выводит байты
--------------------------------------------------------------------*/
void PrintBytes(const wchar_t* aStr, size_t aCount);

std::string DecimalToHex(int aiDecimal);

uint32_t ReadUInt32(const std::vector<uint8_t>& buffer, size_t offset);

uint32_t readU32(const std::vector<uint8_t> &Buffer, size_t Position);

int FindBinaryDataStart(const std::vector<uint8_t>& Data, const std::vector<std::string> &sPackageNames);;

int FindLevelExport(const std::vector<std::unique_ptr<CExportEntry>>& Exports);;

bool bIsSkeletalMesh(const std::string& sClassName);;

float ToSingle(const uint8_t* data, size_t offset = 0);
float ToSingle(const std::vector<uint8_t>& data, size_t offset = 0);

/*--------------------------------------------------------------------
ExportToCSV -- экспортирует данные в csv-таблицу
--------------------------------------------------------------------*/
void ExportToCSV(CMassEffect3Package* PCC);;

void ExportMultipleToCSV(std::vector<ObjectParameters> Parameters);
