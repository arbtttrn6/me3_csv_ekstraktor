#include "objectdef.h"
#include "utils.h"
#include <string>

void ExtractTransform(std::vector<Property> &Props, ObjectParameters &Parameters)
{
	bool bHasLocation = false;
	bool bHasRotation = false;
	bool bHasScale = false;

	for (const auto& SMA_Prop : Props)
	{
		if (strcmp(SMA_Prop.sName.c_str(), "location") == 0)
		{
			Parameters.fLocation[0] = *reinterpret_cast<const float*>(&SMA_Prop.Data[0]);
			Parameters.fLocation[1] = *reinterpret_cast<const float*>(&SMA_Prop.Data[4]);
			Parameters.fLocation[2] = *reinterpret_cast<const float*>(&SMA_Prop.Data[8]);

			bHasLocation = true;

			std::cout << "\nlocation:\n" << std::endl;
			for (int i = 0; i < SMA_Prop.Data.size(); i++)
			{
				printf("%02X ", SMA_Prop.Data[i]);

			}
			std::cout << "\n" << std::endl;
		}
		if (strcmp(SMA_Prop.sName.c_str(), "Rotation") == 0)
		{
			Parameters.fRotation[0] = (*reinterpret_cast<const int*>(&SMA_Prop.Data[0])) * 360 / 65536;
			Parameters.fRotation[1] = (*reinterpret_cast<const int*>(&SMA_Prop.Data[4])) * 360 / 65536;
			Parameters.fRotation[2] = (*reinterpret_cast<const int*>(&SMA_Prop.Data[8])) * 360 / 65536;

			std::cout << "\nRotation:\n" << std::endl;
			for (int i = 0; i < SMA_Prop.Data.size(); i++)
			{
				printf("%02X ", SMA_Prop.Data[i]);

			}
			std::cout << "\n" << std::endl;

			bHasRotation = true;
		}

		if (strcmp(SMA_Prop.sName.c_str(), "DrawScale3D") == 0)
		{
			Parameters.fScale[0] = *reinterpret_cast<const float*>(&SMA_Prop.Data[0]);
			Parameters.fScale[1] = *reinterpret_cast<const float*>(&SMA_Prop.Data[4]);
			Parameters.fScale[2] = *reinterpret_cast<const float*>(&SMA_Prop.Data[8]);

			bHasScale = true;
		}

		if (!bHasLocation)
		{
			Parameters.fLocation[0] = Parameters.fLocation[1] = Parameters.fLocation[2] = 0.0f;
		}
		if (!bHasRotation)
		{
			Parameters.fRotation[0] = Parameters.fRotation[1] = Parameters.fRotation[2] = 0.0f;
		}
		if (!bHasScale)
		{
			Parameters.fScale[0] = Parameters.fScale[1] = Parameters.fScale[2] = 1.0f;
		}

	}

}

std::vector<ObjectParameters> ExtractObjectParameters(
										 int iObjectIndex,
										 const std::vector<std::unique_ptr<CExportEntry>>& PackageExports,
										 const std::vector<std::string>& PackageNames,
										 const std::string sFileName
													)
{

	ObjectParameters Parameters;
	std::vector<ObjectParameters> Objects;

	auto& ExportEntry = PackageExports[iObjectIndex];

	if (ExportEntry)
	{

		if (strcmp(ExportEntry->GetClassName().c_str(), "StaticMeshCollectionActor") == 0)
		{

			std::cerr << "\nExtractObjectParameters: StaticMeshCollectionActor найден." << std::endl;

			ExportInfoOutput(ExportEntry, iObjectIndex);

			auto Props = ExportEntry->GetProperties();
			for (const auto& ComponentsProp : Props)
			{
				if (strcmp(ComponentsProp.sName.c_str(), "StaticMeshComponents") == 0)
				{
					std::cerr << "ExtractObjectParameters: StaticMeshComponents найден." << std::endl;

					std::vector<ObjectProperty> Result;
					int iPos = 0;

					int32_t iCount = *(int32_t*)&ComponentsProp.Data[iPos];
					int iArrayDataPos = iPos + 4;

					std::cerr << "\nStaticMeshComponents: "
							  << "\nType = " << GetTypeAsString(ComponentsProp.sType)
							  << "\nCount = " << iCount
							  << std::endl;

					for (int i = 0; i < iCount; i++)
					{
						int32_t iExportId = *(int32_t*)&ComponentsProp.Data[iArrayDataPos]; // -1

						std::cerr << "[" << i << "] ExportId = " << iExportId << std::endl;

						Result.push_back({iExportId});
						iArrayDataPos += 4;
					};


					for (const auto& Obj : Result)
					{
						if (Obj.iValue > 0)
						{
							int iExportIndex = Obj.iValue - 1;

							auto& ComponentExport = PackageExports[iExportIndex];
							std::vector<int> SMA_List;
							auto SMAProperties = ComponentExport->GetProperties();

							if (ComponentExport)
							{
								ExportInfoOutput(ComponentExport, iExportIndex);

								if (strcmp(ComponentExport->GetObjectName().c_str(), "StaticMeshActor_SMC") == 0)
								{
									SMA_List.push_back({iExportIndex});
								};
							};

							for (const auto& SMAProp : SMAProperties)
							{

								std::cout << SMAProp.sName << std::endl;
								std::cout << "\nHEX:\n" << std::endl;
								for (int i = 0; i < SMAProp.Data.size(); i++)
								{
									printf("%02X ", SMAProp.Data[i]);

								}
								std::cout << "\n" << std::endl;

								if (strcmp(SMAProp.sName.c_str(), "StaticMesh") == 0)
								{

									int32_t iMeshExportId = *reinterpret_cast<const int32_t*>(SMAProp.Data.data());
									if (iMeshExportId > 0)
									{
										int MeshIndex = iMeshExportId - 1;
										auto& MeshExport = PackageExports[MeshIndex];

										ExportInfoOutput(MeshExport, MeshIndex);

										Parameters.iId = iExportIndex + 1;
										Parameters.sName = MeshExport->GetObjectName();
										Parameters.sPackageName = sFileName;
										Parameters.bIsSkeletal = bIsSkeletalMesh(MeshExport->GetClassName());

										int iComponentIndex = -1;

										for (int i = 0; i < iCount; i++)
										{
											if (Result[i].iValue == iExportIndex + 1)
											{
												iComponentIndex = i;
												break;
											}
										};

										ExtractTransformFromStaticMeshCollectionActor(ExportEntry, Parameters, iComponentIndex);

										std::cout << Parameters.iId << ";" << Parameters.sName.c_str() << ";" <<
													 Parameters.fLocation[0] << ";" << Parameters.fLocation[1] << ";" <<
													 Parameters.fLocation[2] << ";" << Parameters.fRotation[0] << ";" <<
													 Parameters.fRotation[1] << ";" << Parameters.fRotation[2] << ";" <<
													 Parameters.fScale[0] << ";" << Parameters.fScale[1] << ";" <<
													 Parameters.fScale[2] << ";" << Parameters.sPackageName << ";" <<
													 Parameters.bIsSkeletal << "\n" << std::endl;

										std::cout << "\nLocX: " <<
													 Parameters.fLocation[0] << ";\nLocY: " <<
													 Parameters.fLocation[1] << ";\nLocZ: " <<
													 Parameters.fLocation[2] << ";\nPitch: " <<
													 Parameters.fRotation[0] << ";\nYaw: " <<
													 Parameters.fRotation[1] << ";\nRoll: " <<
													 Parameters.fRotation[2] << ";\nDrawScale3D X: " <<
													 Parameters.fScale[0] << ";\nDrawScale3D Y: " <<
													 Parameters.fScale[1] << ";\nDrawScale3D Z: " <<
													 Parameters.fScale[2] << ";\n" << std::endl;

										Objects.push_back(Parameters);
									};

								};

							};
						}
					};


				}
			}


		};



		if (strcmp(ExportEntry->GetClassName().c_str(), "StaticMeshActor") == 0)
		{
			std::cerr << "\nExtractObjectParameters: StatcMeshActor найден." << std::endl;

			ExportInfoOutput(ExportEntry, iObjectIndex);

			auto Props = ExportEntry->GetProperties();
			std::cerr << "Получено свойств: " << Props.size() << std::endl;

			for (const auto& Prop : Props) {
				std::cerr << "PropName: " << Prop.sName << std::endl;
				if (strcmp(Prop.sName.c_str(), "StaticMeshComponent") == 0)
				{
					std::cerr << "ExtractObjectParameters: StaticMeshComponent найден." << std::endl;

					int32_t iComponentId = *(int32_t*)&Prop.Data[0];
					if (iComponentId > 0)
					{
						int iComponentIndex = iComponentId - 1;
						auto& ComponentExport = PackageExports[iComponentIndex];

						if (ComponentExport)
						{
							ExportInfoOutput(ComponentExport, iComponentIndex);
						};

						auto ComponentProps = ComponentExport->GetProperties();
						for (const auto& ComProp : ComponentProps)
						{
							if (strcmp(ComProp.sName.c_str(), "StaticMesh") == 0)
							{
								int32_t iMeshId = *reinterpret_cast<const int32_t*>(ComProp.Data.data());
								std::cout << "MeshId: " << iMeshId << std::endl;
								if (iMeshId > 0)
								{
									std::cout << "\nHEX:\n" << std::endl;
									for (int i = 0; i < ComProp.Data.size(); i++)
									{
										printf("%02X ", ComProp.Data[i]);

									}
									std::cout << "\n" << std::endl;

									int iMeshIndex = iMeshId - 1; //-1
									auto& MeshExport = PackageExports[iMeshIndex];

									std::cout << "iMeshIndex: " << iMeshIndex << std::endl;

									std::cout << "Export " << iMeshIndex << " contains: "
											  << MeshExport->GetObjectName() << std::endl;

									ExportInfoOutput(MeshExport, iMeshIndex);

									Parameters.iId = iObjectIndex + 1;
									Parameters.sName = MeshExport->GetObjectName();
									Parameters.sPackageName = sFileName;
									Parameters.bIsSkeletal = false;

									ExtractTransform(Props, Parameters);

									std::cout << Parameters.iId << ";" << Parameters.sName.c_str() << ";" <<
												 Parameters.fLocation[0] << ";" << Parameters.fLocation[1] << ";" <<
												 Parameters.fLocation[2] << ";" << Parameters.fRotation[0] << ";" <<
												 Parameters.fRotation[1] << ";" << Parameters.fRotation[2] << ";" <<
												 Parameters.fScale[0] << ";" << Parameters.fScale[1] << ";" <<
												 Parameters.fScale[2] << ";" << Parameters.sPackageName << ";" <<
												 Parameters.bIsSkeletal << "\n" << std::endl;

									std::cout << "\nLocX: " <<
												 Parameters.fLocation[0] << ";\nLocY: " <<
												 Parameters.fLocation[1] << ";\nLocZ: " <<
												 Parameters.fLocation[2] << ";\nPitch: " <<
												 Parameters.fRotation[0] << ";\nYaw: " <<
												 Parameters.fRotation[1] << ";\nRoll: " <<
												 Parameters.fRotation[2] << ";\nDrawScale3D X: " <<
												 Parameters.fScale[0] << ";\nDrawScale3D Y: " <<
												 Parameters.fScale[1] << ";\nDrawScale3D Z: " <<
												 Parameters.fScale[2] << ";\n" << std::endl;

									Objects.push_back(Parameters);
								}
							}
						}
					}
				}
			}
		};

	}
	return Objects;
}

std::vector<ObjectParameters> ExtractObjectsFromLevel(const std::vector<std::string> &sPackageNames,
													  const std::vector<std::unique_ptr<CImportEntry> > &PackageImports,
													  const std::vector<std::unique_ptr<CExportEntry> > &PackageExports,
													  const std::string sFileName) {

	std::vector<ObjectParameters> Result;

	int iLevelIndex = FindLevelExport(PackageExports);
	if (iLevelIndex == -1) { return Result; };

	auto LevelData = PackageExports[iLevelIndex]->GetData();

	int iBinaryOffset = FindBinaryDataStart(LevelData, sPackageNames);
	std::cout << "ExtractObjectsFromLevel: iBinaryOffset = " << iBinaryOffset << std::endl;

	if (iBinaryOffset == 0)
	{
		std::cerr << "ExtractObjectsFromLevel: Не найдено смещение двоичных данных Level." << std::endl;
		return Result;
	};

	int iPosition = iBinaryOffset + 4;
	std::cout << "ExtractObjectsFromLevel: iPosition = " << iPosition << std::endl;
	int iObjectCount;

	// Чтение iObjectCount
	if (iPosition + 4 <= LevelData.size())
	{
		iObjectCount = (static_cast<uint32_t>(LevelData[iPosition])) |
				(static_cast<uint32_t>(LevelData[iPosition + 1]) << 8) |
				(static_cast<uint32_t>(LevelData[iPosition + 2]) << 16) |
				(static_cast<uint32_t>(LevelData[iPosition + 3]) << 24);
	}
	else
	{
		throw std::out_of_range("Buffer overflow. Offset = " +
		std::to_string(iPosition) + ", available = " +
		std::to_string(LevelData.size() - iPosition));
	};

	std::cout << "ExtractObjectsFromLevel: iObjectCount = " << iObjectCount << std::endl;
	if (iObjectCount == 0)
	{
		return Result;
	};

	std::vector<int> iObjectIndices;

	for (int i = 0; i < iObjectCount; i++)
	{
		int idx;

		if (iPosition + 4 <= LevelData.size())
		{
			idx = (static_cast<uint32_t>(LevelData[iPosition])) |
					(static_cast<uint32_t>(LevelData[iPosition + 1]) << 8) |
					(static_cast<uint32_t>(LevelData[iPosition + 2]) << 16) |
					(static_cast<uint32_t>(LevelData[iPosition + 3]) << 24);
		}
		else
		{
			throw std::out_of_range("Buffer overflow. Offset = " +
									std::to_string(iPosition) + ", available = " +
									std::to_string(LevelData.size() - iPosition));
		};

		idx -= 1;


		iPosition += 4;
		if (idx > 0 && idx < PackageExports.size())
		{
			iObjectIndices.push_back(idx);
		};

		std::cout << "ExtractObjectsFromLevel: iPosition = " << iPosition << std::endl;
	};

	for (int idx : iObjectIndices)
	{
		if (PackageExports[idx]->GetObjectName().empty() ||
				PackageExports[idx]->GetClassName().empty())
		{
			continue;
		}
		std::vector<ObjectParameters> params = ExtractObjectParameters(idx, PackageExports, sPackageNames, sFileName);
		std::cout << "ExtractObjectsFromLevel: Параметры обработаны" << std::endl;

		Result.reserve(params.size());
		std::move(params.begin(), params.end(), std::inserter(Result, Result.end()));
		params.clear();
		std::cout << "ExtractObjectsFromLevel: Параметры занесены в массив" << std::endl;
	};

	if (iObjectIndices.empty())
	{
		std::cerr << "ExtractObjectsFromLevel: Массив пуст!" << std::endl;
	};

	return Result;
}

Property GetProp(std::vector<Property> Props, std::string sPropName)
{
	for (auto& Prop : Props)
	{
		if (strcmp(Prop.sName.c_str(), sPropName.c_str()) == 0)
		{
			return Prop;
		};
	};

};

void ExportInfoOutput(auto& ExportEntry, int iObjectIndex)
{
	iObjectIndex += 1;
	std::cout << "\nExport Info: "
			  << "\n\t(Exp) " << iObjectIndex << " : " << ExportEntry->GetClassName() << " (" << ExportEntry->GetObjectName() << ")"
			  << "\n\tExport Header: "
			  << "\n\tExport Index = " << iObjectIndex
			  << "\n\t[0x10] Object Index = " << ExportEntry->GetIndexValue()
			  << "\n\t[0x0C] Object Name = " << ExportEntry->GetObjectName()
			  << "\n\t[0x00] Class = " << ExportEntry->GetClassName()
			  << "\n\t[0x04] Super Class = " << ExportEntry->GetSuperClass()
			  << "\n\t[0x08] Package Name = " << ExportEntry->GetPackageFullName()
			  << "\n\t[0x14] Archetype Name = " << ExportEntry->GetArchtypeName() << std::endl;
	if (ExportEntry->GetIdxArchtype() != 0)
	{
		CIEntry* Archtype = ExportEntry->GetProvider()->GetEntry(ExportEntry->GetIdxArchtype());
		std::string sFullInfo = Archtype->GetPackageFullName() + "." + Archtype->GetObjectName();

		std::cout << sFullInfo << " ("
				  << (ExportEntry->GetIdxArchtype() < 0 ? "imported" : "local")
				  << ") "
				  << (ExportEntry->GetIdxArchtype())
				  << std::endl;
	}
	std::cout << ""
			  << "\n\tHeader Size = " << ExportEntry->GetHeader().size() << " bytes"
			  << "\nExport Data: "
			  << "\n\t[0x20] Data Size = " << ExportEntry->GetDataSize() << " bytes"
			  << "\n\t[0x24] Data Offset Hex = " << "0x" << std::hex << ExportEntry->GetDataOffset()
			  << "\n\t[0x24] Data Offset Dec = " << std::dec << ExportEntry->GetDataOffset()
			  << "\n\t[0x28] Export Flags = " << ExportEntry->GetObjectFlags()
			  << std::endl;
}

void ExtractTransformFromStaticMeshCollectionActor(auto& ExportEntry, ObjectParameters &Parameters, int aiComponentIndex)
{

	int iStart = ExportEntry->GetPropsEndOffset();
	int iComponentOffset = iStart + (aiComponentIndex * 64);

	DecomposeTransformMatrix(ExportEntry->GetData(), iComponentOffset, Parameters);
}

void ExtractEulerAnglesYXZ(const Matrix4x4& m, Vector3& rotation)
{
	float sy = sqrt(m.m[0][0] * m.m[0][0] + m.m[1][0] * m.m[1][0]);

	bool singular = sy < 1e-6f;

	if (!singular)
	{
		rotation.fX = atan2(m.m[2][0], sy);        // Yaw
		rotation.fY = atan2(m.m[1][0], m.m[0][0]);  // Roll
		rotation.fZ = atan2(m.m[2][1], m.m[2][2]);  // Pitch
	} else
	{
		float cosYaw = m.m[0][0] / sy;
		float sinYaw = -m.m[1][0] / sy;
		rotation.fX = atan2(sinYaw, cosYaw);
//		rotation.fX = atan2(m.m[2][0], sy);        // Yaw
		rotation.fY = 0;                            // Roll
		rotation.fZ = atan2(-m.m[1][2], m.m[1][1]); // Pitch
	}

	// Конвертование в градусы
	const float toDegrees = 180.0f / M_PI;
	rotation.fX = round(rotation.fX * toDegrees * 1000) / 1000;
	rotation.fY = round(rotation.fY * toDegrees * 1000) / 1000;
	rotation.fZ = round(rotation.fZ * toDegrees * 1000) / 1000;

//	rotation.fX = rotation.fX * 180.0f / M_PI;
//	rotation.fY = rotation.fY * 180.0f / M_PI;
//	rotation.fZ = rotation.fZ * 180.0f / M_PI;
}

void DecomposeTransformMatrix(const std::vector<uint8_t> &avData, int aiComponentIndex, ObjectParameters &Params)
{
	int baseOffset = aiComponentIndex;

	Params.fLocation[0] = *reinterpret_cast<const float*>(&avData[baseOffset + 12 * 4]);
	Params.fLocation[1] = *reinterpret_cast<const float*>(&avData[baseOffset + 13 * 4]);
	Params.fLocation[2] = *reinterpret_cast<const float*>(&avData[baseOffset + 14 * 4]);

	Matrix4x4 transformMatrix(avData, baseOffset);

	Params.fScale[0] = std::sqrt(transformMatrix.m[0][0] * transformMatrix.m[0][0] +
			transformMatrix.m[0][1] * transformMatrix.m[0][1] +
			transformMatrix.m[0][2] * transformMatrix.m[0][2]);

	Params.fScale[1] = std::sqrt(transformMatrix.m[1][0] * transformMatrix.m[1][0] +
			transformMatrix.m[1][1] * transformMatrix.m[1][1] +
			transformMatrix.m[1][2] * transformMatrix.m[1][2]);

	Params.fScale[2] = std::sqrt(transformMatrix.m[2][0] * transformMatrix.m[2][0] +
			transformMatrix.m[2][1] * transformMatrix.m[2][1] +
			transformMatrix.m[2][2] * transformMatrix.m[2][2]);

	Matrix4x4 rotationMatrix = transformMatrix;
	for (int i = 0; i < 3; i++)
	{
		rotationMatrix.m[i][0] /= Params.fScale[i];
		rotationMatrix.m[i][1] /= Params.fScale[i];
		rotationMatrix.m[i][2] /= Params.fScale[i];
	}

	Vector3 rotation;
	ExtractEulerAnglesYXZ(rotationMatrix, rotation);

	Params.fRotation[0] = rotation.fX; // Pitch
	Params.fRotation[1] = rotation.fY; // Yaw
	Params.fRotation[2] = rotation.fZ; // Roll
}
