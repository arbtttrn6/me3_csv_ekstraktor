#ifndef PROPERTYTYPEDEF_H
#define PROPERTYTYPEDEF_H
#include <string>
#pragma once

/*--------------------------------------------------------------------
PropertyType -- типы параметров
--------------------------------------------------------------------*/
enum class PropertyType
{
	None,
	StructProperty,
	IntProperty,
	FloatProperty,
	ObjectProperty,
	NameProperty,
	BoolProperty,
	ByteProperty,
	ArrayProperty,
	Unknown,
};

PropertyType GetType(std::string sTypeName);
std::string GetTypeAsString(PropertyType aType);

#endif // PROPERTYTYPEDEF_H
