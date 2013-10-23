/*
 * Field.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: Kamal Sharma
 */

#include "Field.hpp"

/**
 * Returns the field block
 * @return
 */
FieldBlock*& Field::getFieldBlock() {
	return fieldBlock;
}

/**
 * Sets the field block
 * @param fieldBlock
 */
void Field::setFieldBlock(FieldBlock* fieldBlock) {
	this->fieldBlock = fieldBlock;
}

/**
 * Returns the field name
 * @return
 */
const string& Field::getName() {
	return name;
}

/**
 * Sets the field name
 * @param name
 */
void Field::setName(string& name) {
	this->name = name;
}

/**
 * Returns the field type
 * @return
 */
FieldTypes::TYPE Field::getType() {
	return type;
}

/**
 * Sets the field type
 * @param type
 */
void Field::setType(FieldTypes::TYPE type) {
	this->type = type;
}

/**
 * Sets the position
 * @param position
 */
void Field::setPosition(int position) {
	this->position = position;
}

/**
 * Returns the position
 * @return
 */
int Field::getPosition() {
	return position;
}

/**
 * Returns the type string
 * @return
 */
string Field::getTypeString()
{
	string typestr;
	switch(type)
	{
	case FieldTypes::INT : typestr = "int" ; break;
	case FieldTypes::FLOAT : typestr = "float"; break;
	case FieldTypes::DOUBLE : typestr = "double"; break;
	default : traceerror << " Unknown Type found " << endl ; exit(1);
	}
	return typestr;
}

/**
 * Returns the element size
 * based on the field type
 * @return
 */
int Field::getElementSize()
{
	int size;
	trace << " Field name:" << name << endl ;
	switch(type)
	{
	case FieldTypes::INT : size = sizeof(int); break;
	case FieldTypes::FLOAT : size = sizeof(float); break;
	case FieldTypes::DOUBLE : size = sizeof(double); break;
	default : traceerror << " Unknown Type found " << endl ; exit(1);
	}
	return size;
}

/**
 * Prints a field
 */
void Field::print()
{
	trace << " Field name:" << name ;
	switch(type)
	{
	case FieldTypes::INT : trace << " INT "; break;
	case FieldTypes::FLOAT : trace << " FLOAT "; break;
	case FieldTypes::DOUBLE : trace << " DOUBLE "; break;
	default : trace << " Unknown Type found " << endl ; exit(1);
	}
	trace << endl;
}

/**
 * Unparses a field to string
 */
string Field::unparseToString()
{
	string result = name ;
	switch(type)
	{
	case FieldTypes::INT : result += " INT "; break;
	case FieldTypes::FLOAT : result += " FLOAT "; break;
	case FieldTypes::DOUBLE : result += " DOUBLE "; break;
	default : trace << " Unknown Type found " << endl ; exit(1);
	}
	return result;
}
