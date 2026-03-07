#include "cmasseffect3package.h"
#include <ostream>
#include "utils.h"

/*--------------------------------------------------------------------
me3_csv_ekstraktor -- достаёт из PCC файлов Mass Effect 3 координаты
моделей для их разложения в 3D редакторе как на внутриигровых уровнях.

Artur Ajvazjan (arbtttrn6), 2025-2026.

В качестве материалов-ориентиров были взяты:

* https://github.com/ME3Explorer/ME3Explorer/
* https://github.com/ME3Tweaks/LegendaryExplorer/
* https://me3explorer.fandom.com/wiki/

!!!ОСТОРОЖНО!!! НИЗКОКАЧЕСТВЕННЫЙ КОД
--------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	// Получение имени pcc-файла
	std::string sPccFile = "";
	std::vector<std::string> PccFiles;
	bool bShowFileInfo = false;
	bool bMultipleInOne = false;

	if (argc > 1)
	{
		if (argc == 3)
		{
			for (int i = 1; i < argc; ++i)
			{
				std::string arg = argv[i];
				if (arg == "-i" || arg == "-I")
				{
					bShowFileInfo = true;
				}
				else if (arg == "-p" || arg == "-P")
				{
					bMultipleInOne = true;
				}
			};

			for (int i = 1; i < argc; ++i)
			{
				std::string arg = argv[i];
				if (arg != "-i" && arg != "-I" &&
					arg != "-p" && arg != "-P")
				{
					sPccFile = arg;
				}
			}
		}
		else if (argc >= 3)
		{
			for (int i = 1; i < argc; ++i)
			{
				std::string arg = argv[i];
				if (arg == "-i" || arg == "-I")
				{
					bShowFileInfo = true;
					break;
				}
				if (arg == "-p" || arg == "-P")
				{
					bMultipleInOne = true;
					break;
				}
			};

			for (int i = 1; i < argc; ++i)
			{
				std::string arg = argv[i];
				if (arg != "-i" && arg != "-I" &&
					arg != "-p" && arg != "-P")
				{
					PccFiles.push_back(arg);
				}
			}
		}
		else if (argc == 2)
		{
			sPccFile = argv[1];
		}
		else
		{
			std::cout << "Использование: " << argv[0] << " <file.pcc>" << std::endl;
			std::cout << "Пример: " << argv[0] << " BioA_MPDish_200.pcc" << std::endl;
			std::cout << "\n" << "Для вывода инфы о файле параметр -i" << std::endl;
			std::cout << "\n" << "Для включения данных подкарт в один файл параметр -p" << std::endl;
			return 1;
		};
	}
	else
	{
		std::cout << "Использование: " << argv[0] << " <file.pcc>" << std::endl;
		std::cout << "Пример: " << argv[0] << " BioA_MPDish_200.pcc" << std::endl;
		std::cout << "\n" << "Для вывода инфы о файле параметр -i" << std::endl;
		std::cout << "\n" << "Для включения данных подкарт в один файл параметр -p" << std::endl;
		return 1;
	}

	if (!PccFiles.empty())
	{
		if (bMultipleInOne == false)
		{
			for (const std::string& sFileName : PccFiles)
			{
				CMassEffect3Package* Pcc = new CMassEffect3Package(sFileName);

				if (bShowFileInfo)
				{
					Pcc->PackageFileInfo();
				}
				else
				{
					Pcc->ToRead();
					// Экспорт в csv
					ExportToCSV(Pcc);
				};
			}
		}
		else if (bMultipleInOne == true)
		{
			std::vector<ObjectParameters> ParametersOfMultiplies;
			for (const std::string& sFileName : PccFiles)
			{
				std::cout << "Обработка файла: " << sFileName << std::endl;
				CMassEffect3Package* Pcc = new CMassEffect3Package(sFileName);
				Pcc->ToRead();
				std::vector<ObjectParameters> Params = Pcc->GetObjectParameters();
				if (!Params.empty())
				{
					ParametersOfMultiplies.reserve(ParametersOfMultiplies.size() + Params.size());
					ParametersOfMultiplies.insert(ParametersOfMultiplies.end(), Params.begin(), Params.end());
				}
			};

			if (!ParametersOfMultiplies.empty())
			{
				ExportMultipleToCSV(ParametersOfMultiplies);
			}
			else
			{
				std::cerr << "Файлы не содержат данных." << std::endl;
			}
		}
		return 1;
	}
	else
	{
		// Задача pcc-пакета
		CMassEffect3Package* Pcc = new CMassEffect3Package(sPccFile);

		if (bShowFileInfo)
		{
			Pcc->PackageFileInfo();
		}
		else
		{
			Pcc->ToRead();
			// Экспорт в csv
			ExportToCSV(Pcc);
		};
	}

	return 0;
}
