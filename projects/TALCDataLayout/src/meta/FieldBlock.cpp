/*
 * FieldBlock.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: Kamal Sharma
 */

#include "FieldBlock.hpp"

/**
 * Inserts a field in a field block
 * @param field
 */
void FieldBlock::addField(Field *field)
{
	field->setFieldBlock(this);
	fieldList.push_back(field);
}

/**
 * Sets the view of the Field Block
 * @param view
 */
void FieldBlock::setView(View* view)
{
	this->view = view;
}

/**
 * Returns field vector in Field Block
 * @return Field Vector
 */
vector<Field*> FieldBlock::getFieldList()
{
	return fieldList;
}

/**
 * Returns the view of field block
 * @return view
 */
View* FieldBlock::getView()
{
	return view;
}

/**
 * Sets the block size parameter
 * @param size
 */
void FieldBlock::setBlockSize(int size)
{
	blocksize = size;
}

/**
 * Returns the block size for
 * current field block
 * @return
 */
int FieldBlock::getBlockSize()
{
	return blocksize;
}

///**
// * Sets the field block number
// * @param fieldBlockNo
// */
//void FieldBlock::setFieldBlockNo(int fieldBlockNo)
//{
//	this->fieldBlockNo = fieldBlockNo;
//}
//
///**
// * Returns the field block number
// * @return
// */
//int FieldBlock::getFieldBlockNo()
//{
//	return fieldBlockNo;
//}

/**
 * Checks if this field block contains
 * a single field. Useful for checking
 * whether to build struct
 * @return
 */
bool FieldBlock::isSingleFieldBlock()
{
	if(fieldList.size() == 1)
		return true;
	else
		return false;
}

/**
 * Checks if field block is blocked
 * to create arrays in structs
 * @return true if blocksize > 1, else
 * 			false
 */
bool FieldBlock::isBlocked()
{
	if(blocksize > 1)
		return true;
	else
		return false;
}

/**
 * Returns the number of fields in this
 * field block
 * @return
 */
int FieldBlock::getSize()
{
	return fieldList.size();
}

/**
 * Prints a Field Block
 */
void FieldBlock::print(){
	trace << "FieldBlock: <num elements:" << fieldList.size() << ">" << std::endl;
	foreach(Field *field, fieldList)
	{
		field->print();
	}
	trace << " BlockSize: " << blocksize << std::endl;
}

/**
 * Unparses a Field Block
 */
string FieldBlock::unparseToString(){
	string result = "Field { ";

	foreach(Field *field, fieldList)
	{
		result += field->unparseToString() + " ";
	}
	result += " }";

	return result;
}
