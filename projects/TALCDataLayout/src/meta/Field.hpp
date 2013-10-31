/*
 * Field.h
 *
 *  Created on: Oct 11, 2012
 *      Author: kamalsharma
 */

#ifndef FIELD_H_
#define FIELD_H_

#include <string>
#include <vector>

#include "Common.hpp"

using namespace std;
class FieldBlock;

class Field {
public:
	Field(string name = "", FieldTypes::TYPE type = FieldTypes::UNKNOWN,
			FieldBlock *fieldBlock = NULL, int position = 0) :
			name(name), type(type), fieldBlock(fieldBlock), position(position) {
	}

	FieldBlock*& getFieldBlock();
	void setFieldBlock(FieldBlock* fieldBlock);

	const string& getName();
	void setName(string& name);

	FieldTypes::TYPE getType();
	void setType(FieldTypes::TYPE type);

	int getPosition();
	void setPosition(int position);

	int getElementSize();
	string getTypeString();

	void print();

	// Utility
	string unparseToString();

private:
	string name;
	FieldTypes::TYPE type;
	FieldBlock *fieldBlock;
	// Position of Field in Field Block
	// This is useful for offset calculation
	int position;
};

#endif /* FIELD_H_ */
