/*
 * TypeSystem.cpp
 *
 *  Created on: Jul 2, 2009
 *      Author: panas2
 */
#include <sstream>
#include <iostream>
#include "TypeSystem.h"
using namespace std;

/**********************************************
 * ResolveType: For a given type represented as a string
 * this function returns the type of this string, including
 * the size in bytes of the type
 * @param: string typeStr - type represented as a string
 */
TypeInfo* TypeSystem::resolveType(string typeStr) {
	TypeInfo* tinf = NULL;
	if (typeStr=="SgTypeBool") {
		tinf=new TypeInfo(SgTypeBool, typeStr, sizeof(bool));
	} else if (typeStr=="SgTypeChar") {
		tinf=new TypeInfo(SgTypeChar, typeStr, sizeof(char));
	} else if (typeStr=="SgTypeDouble") {
		tinf=new TypeInfo(SgTypeDouble, typeStr, sizeof(double));
	} else if (typeStr=="SgTypeFloat") {
		tinf=new TypeInfo(SgTypeFloat, typeStr, sizeof(float));
	} else if (typeStr=="SgTypeInt") {
		tinf=new TypeInfo(SgTypeInt, typeStr, sizeof(int));
	} else if (typeStr=="SgTypeLong") {
		tinf=new TypeInfo(SgTypeLong, typeStr, sizeof(long));
	} else if (typeStr=="SgTypeLongDouble") {
		tinf=new TypeInfo(SgTypeLongDouble, typeStr, sizeof(long double));
	} else if (typeStr=="SgTypeLongLong") {
		tinf=new TypeInfo(SgTypeLongLong, typeStr, sizeof(long long));
	} else if (typeStr=="SgTypeShort") {
		tinf=new TypeInfo(SgTypeShort, typeStr, sizeof(short));
	} else if (typeStr=="SgTypeSignedChar") {
		tinf=new TypeInfo(SgTypeSignedChar, typeStr, sizeof(signed char));
	} else if (typeStr=="SgTypeSignedInt") {
		tinf=new TypeInfo(SgTypeSignedInt, typeStr, sizeof(signed int));
	} else if (typeStr=="SgTypeSignedLong") {
		tinf=new TypeInfo(SgTypeSignedLong, typeStr, sizeof(signed long));
	} else if (typeStr=="SgTypeSignedLongLong") {
		tinf=new TypeInfo(SgTypeSignedLongLong, typeStr, sizeof(signed long long));
	} else if (typeStr=="SgTypeSignedShort") {
		tinf=new TypeInfo(SgTypeSignedShort, typeStr, sizeof(signed short));
	} else if (typeStr=="SgTypeUnsignedChar") {
		tinf=new TypeInfo(SgTypeUnsignedChar, typeStr, sizeof(unsigned char));
	} else if (typeStr=="SgTypeUnsignedInt") {
		tinf=new TypeInfo(SgTypeUnsignedInt, typeStr, sizeof(unsigned int));
	} else if (typeStr=="SgTypeUnsignedLong") {
		tinf=new TypeInfo(SgTypeUnsignedLong, typeStr, sizeof(unsigned long));
	} else if (typeStr=="SgTypeUnsignedLongLong") {
		tinf=new TypeInfo(SgTypeUnsignedLongLong, typeStr, sizeof(unsigned long long));
	} else if (typeStr=="SgTypeUnsignedShort") {
		tinf=new TypeInfo(SgTypeUnsignedShort, typeStr, sizeof(unsigned short));
	} else if (typeStr=="SgTypeString") {
		tinf=new TypeInfo(SgTypeString, typeStr, sizeof(string));
	} else if (typeStr=="SgPointerType") {
		tinf=new TypeInfo(SgPointerType, typeStr, sizeof(void*));
	} else {
		tinf=new TypeInfo(Unknown, "Unknown", 0);
	}
	return tinf;
}



