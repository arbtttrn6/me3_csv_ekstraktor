#include "cme3exportentry.h"

/*--------------------------------------------------------------------
ME3ExportEntry -- класс экспортов Mass Effect 3
--------------------------------------------------------------------*/
CME3ExportEntry::CME3ExportEntry(CIEntryProvider* aProvider, ZBlock::BinaryReader &stream) :
	CExportEntry(aProvider)
{

	HeaderOffset = stream.tell();

	// Чтение основной инфы заголовка чтобы определить размер
	stream.seek(HeaderOffset + 44);
	int count = stream.readS32();
	stream.seek(HeaderOffset);

//	std::cout << "\n\tCount: " << count << std::endl;

	// Расчёт размера инфы экспорта для ME3
	int expInfoSize = 68 + (count * 4);

	Header.resize(expInfoSize);
	stream.readBytes(Header.data(), expInfoSize);

	OriginalDataSize = GetDataSize();
	long HeaderEnd = stream.tell();

	// Чтение данных экспорта
	stream.seek(GetDataOffset());
	Data.resize(GetDataSize());
	stream.readBytes(Data.data(), GetDataSize());
	stream.seek(HeaderEnd);

	// Определение чтения из конфига
	if ((GetObjectFlags() & 0x02000000) != 0 && Data.size() > 25)
	{
		ReadsFromConfig = (Data[25] & 64) != 0;
	} else
	{
		ReadsFromConfig = false;
	};
}
