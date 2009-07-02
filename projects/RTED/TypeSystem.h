/*
 * TypeSystem.h
 *
 *  Created on: Jul 2, 2009
 *      Author: panas2
 */

#ifndef TYPESYSTEM_H_
#define TYPESYSTEM_H_

/********************************************
 * Enum with all supported types
 */
enum SGTypes {
	SgTypeBool,
	SgTypeChar,
	SgTypeDouble,
	SgTypeFloat,
	SgTypeInt,
	SgTypeLong,
	SgTypeLongDouble,
	SgTypeLongLong,
	SgTypeShort,
	SgTypeSignedChar,
	SgTypeSignedInt,
	SgTypeSignedLong,
	SgTypeSignedLongLong,
	SgTypeSignedShort,
	SgTypeUnsignedChar,
	SgTypeUnsignedInt,
	SgTypeUnsignedLong,
	SgTypeUnsignedLongLong,
	SgTypeUnsignedShort,
	SgTypeString,
	Unknown
};

/********************************************
 * TypeInfo: Contains information about each type
 * including its enum value, string representation and
 * size in bytes
 */
class TypeInfo {
private:
	int sizeBytes;
	SGTypes type;
	std::string typeStr;
public:
	TypeInfo(SGTypes _type, std::string _typeStr, int _size):type(_type),typeStr(_typeStr),sizeBytes(_size){};
	~TypeInfo(){};
	int get_size() {return sizeBytes;}
	std::string get_typeStr() {return typeStr;}
	SGTypes get_type() {return type;}
};

/********************************************
 * TypeSystem:
 * Returns the Type Information of a given type
 * represented as a string
 */
class TypeSystem {
public:
	TypeSystem(){};
	~TypeSystem(){};
	TypeInfo* resolveType(std::string typeStr);
};

#endif /* TYPESYSTEM_H_ */
