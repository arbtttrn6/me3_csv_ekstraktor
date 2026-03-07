#ifndef PROPERTYDEF_H
#define PROPERTYDEF_H

#include "propertytypedef.h"
#include <string>
#include <vector>


/*--------------------------------------------------------------------
struct Property -- структура для хранения параметров.
sName -- имя параметра;
sType -- тип параметра (ObjectProperty, ArrayProperty и т.д.);
Data -- остальные данные параметра;
--------------------------------------------------------------------*/
struct Property
{
	std::string sName;
	PropertyType sType;
	std::vector<uint8_t> Data;
};

/*--------------------------------------------------------------------
struct ObjectProperty -- структура параметра ObjectProperty.
ObjectProperty хранит индекс экспорта или импорта.
--------------------------------------------------------------------*/
struct ObjectProperty
{
	int iValue;
};


/*--------------------------------------------------------------------
struct ArrayProperty -- структура параметра ArrayProperty.
Эта реализация хранит в себе список ObjectProperty-ев.
--------------------------------------------------------------------*/
struct ArrayProperty
{
	std::vector<ObjectProperty> Items;
};

#endif // PROPERTYDEF_H
