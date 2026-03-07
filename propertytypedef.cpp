#include "propertytypedef.h"
#include <cstring>
#include <string>

PropertyType GetType(std::string sTypeName)
{
	if (strcmp(sTypeName.c_str(), "None") == 0) return PropertyType::None;
	if (strcmp(sTypeName.c_str(), "IntProperty") == 0) return PropertyType::IntProperty;
	if (strcmp(sTypeName.c_str(), "FloatProperty") == 0) return PropertyType::FloatProperty;
	if (strcmp(sTypeName.c_str(), "ObjectProperty") == 0) return PropertyType::ObjectProperty;
	if (strcmp(sTypeName.c_str(), "NameProperty") == 0) return PropertyType::NameProperty;
	if (strcmp(sTypeName.c_str(), "BoolProperty") == 0) return PropertyType::BoolProperty;
	if (strcmp(sTypeName.c_str(), "ByteProperty") == 0) return PropertyType::ByteProperty;
	if (strcmp(sTypeName.c_str(), "ArrayProperty") == 0) return PropertyType::ArrayProperty;
	if (strcmp(sTypeName.c_str(), "StructProperty") == 0) return PropertyType::StructProperty;

	return PropertyType::Unknown;
}

std::string GetTypeAsString(PropertyType aType)
{
	if (aType == PropertyType::None) { return "None"; };
	if (aType == PropertyType::IntProperty) { return "IntProperty"; };
	if (aType == PropertyType::ObjectProperty) { return "ObjectProperty"; };
	if (aType == PropertyType::NameProperty) { return "NameProperty"; };
	if (aType == PropertyType::BoolProperty) { return "BoolProperty"; };
	if (aType == PropertyType::ByteProperty) { return "ByteProperty"; };
	if (aType == PropertyType::ArrayProperty) { return "ArrayProperty"; };
	if (aType == PropertyType::StructProperty) { return "StructProperty"; };

	return "Unknown";
}
