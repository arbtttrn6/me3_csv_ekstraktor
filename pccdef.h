#ifndef PCCDEF_H
#define PCCDEF_H
#include <cstdint>

/*--------------------------------------------------------------------
PCCiChunkHeader -- структура заголовка чанков
--------------------------------------------------------------------*/
struct PCCiChunkHeader
{
	int32_t MagicNumber;
	int32_t BlockSize;
	int32_t CompressedSize;
	int32_t UncompressedSize;
};

/*--------------------------------------------------------------------
PCCiBlock -- структура блоков pcc
--------------------------------------------------------------------*/
struct PCCiBlock
{
	int32_t CompressedSize;
	int32_t UncompressedSize;
};

/*--------------------------------------------------------------------
PCCiFileHeader -- заголовок pcc-файла
--------------------------------------------------------------------*/
#pragma pack(push, 1)
struct PCCiFileHeader
{
	uint32_t MagicNumber; // 0x9E2A83C1
	uint32_t Version; // 0xC202AC = 12714668d = 194/684
	uint32_t Unknown1; // 0x67c
	uint32_t Unknown2; // 0xFFFFFFFB = 5
	char16_t None[5]; // 0x4E006F006E006500 (N.o.n.e...)
	uint32_t PackageFlags; // 0x8088000D = 2156396557d
	uint32_t PackageTypeIndicator; // 0 = Normal Package, 1 = Patch Package (only for use in Patch_001.sfar)
	uint32_t NameCount; // 0x23 = 25d
	uint32_t PointerToNameTable; // 0x8E = 142d
	uint32_t ExportCount; // 0x6 = 6d
	uint32_t PointerToExportTable; // 0x4c8 = 1224d
	uint32_t ImportCount; // 0x5 = 5d
	uint32_t PointerToImportTable; // 0x43C = 1084h
	uint32_t Unknown3; // 0x664 = 1636d
	uint32_t Unknown4; // 0x67C = 1660h
	uint32_t Unknown5; // 0x0
	uint32_t Unknown6; // 0x0
	uint32_t Unknown7; // 0x0
	uint32_t Guid1; // 0x1241956A = 306287978d
	uint32_t Guid2; // 0x48ECB3A7 = 1223472039d
	uint32_t Guid3; // 0x8EDAE68B = 2396710539d
	uint32_t Guid4; // 0xB945722C = 3108336172d
	uint32_t Generations; // 0x1
	uint32_t ExportCount2; // 0x6 = 6d
	uint32_t NameCount2; // 0x23 = 35d
	uint32_t Unknown8; // 0x0 = 0d
	uint32_t Engine; // 0x18EF = 6383d
	uint32_t Unknown9; // 0x3006B = 196715d
	uint32_t Unknown10; // 0x15330000 = 355663872d
	uint32_t Unknown11; // 0x8AA0000 = 145358848d
	uint32_t CompressionType; // (0 = None, 1 = Zlib, 2 = LZO). Unused if Package Flags does not set Compressed bit.
	uint32_t ChunkCount; // 0x0 или 0x1
};
#pragma pack(pop)
#endif // PCCDEF_H
