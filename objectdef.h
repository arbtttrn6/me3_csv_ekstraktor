#ifndef OBJECTDEF_H
#define OBJECTDEF_H

#include "cexportentry.h"
#include "cimportentry.h"
#include <memory>
#include <string>
#include <vector>

/*--------------------------------------------------------------------
ObjectParameters -- структура для параметров объектов
--------------------------------------------------------------------*/
struct ObjectParameters
{
	int iId;
	std::string sName;
	float fLocation[3];
	float fRotation[3];
	float fScale[3];
	std::string sPackageName;
	bool bIsSkeletal;
};

/*--------------------------------------------------------------------
ExtractTransform -- извлечение трансформаций объекта (типа StaticMeshActor):

		location -- позиция
		rotation -- вращение
		drawscale3d -- масштаб
--------------------------------------------------------------------*/
void ExtractTransform(
						std::vector<Property>& Props,
						ObjectParameters& Parameters
					);;

struct Vector3
{
	float fX, fY, fZ;
};

struct Matrix4x4
{
	float m[4][4];

	Matrix4x4(const std::vector<uint8_t>& avData, int aiBaseOffset)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				int offset = (i * 4 + j) * 4;
				m[i][j] = *reinterpret_cast<const float*>(&avData[aiBaseOffset + offset]);
			};
		};
	};

};

void DecomposeTransformMatrix(const std::vector<uint8_t>& avData,
							  int aiComponentIndex,
							  ObjectParameters& Params
							  );

void ExtractTransformFromStaticMeshCollectionActor(
												   auto& ExportEntry,
												   ObjectParameters& Parameters,
												   int aiComponentIndex
												   );

/*--------------------------------------------------------------------
ExtractObjectParameters -- извлечение параметров объекта
--------------------------------------------------------------------*/
std::vector<ObjectParameters> ExtractObjectParameters(
		int iObjectIndex,
		const std::vector<std::unique_ptr<CExportEntry>>& PackageExports,
		const std::vector<std::string>& PackageNames,
		const std::string sFileName
	);;


/*--------------------------------------------------------------------
ExtractObjectsFromLevel -- извлечение объектов из уровня
--------------------------------------------------------------------*/
std::vector<ObjectParameters> ExtractObjectsFromLevel(
	const std::vector<std::string>& sPackageNames,
	const std::vector<std::unique_ptr<CImportEntry>>& PackageImports,
	const std::vector<std::unique_ptr<CExportEntry>>& PackageExports,
	const std::string sFileName);;

/*--------------------------------------------------------------------
GetProp -- для получения Prop из Props
--------------------------------------------------------------------*/
Property GetProp(std::vector<Property> Props, std::string sPropName);

void ExportInfoOutput(auto& ExportEntry, int iObjectIndex);

#endif // OBJECTDEF_H
