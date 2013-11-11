/*
 * FieldBlock.hpp
 *
 *  Created on: Oct 15, 2012
 *      Author: Kamal Sharma
 */

#ifndef FIELDBLOCK_HPP_
#define FIELDBLOCK_HPP_

#include <string>
#include <vector>

#include "Field.hpp"

using namespace std;
class View;

class FieldBlock
{
public:
	FieldBlock() :view(NULL), blocksize(0) {};

	void addField(Field *field);
	bool isSingleFieldBlock();

	// View Methods
	void setView(View* view);
	View* getView();

	// Field Methods
	vector<Field*> getFieldList();

	// Block size
	void setBlockSize(int size);
	int getBlockSize();
	bool isBlocked();

	// Returns the fieldlist size
	int getSize();

	// Print
	void print();

	// Utility
	string unparseToString();

private:
	vector<Field*> fieldList;
	View *view;
	int blocksize;
	//int fieldBlockNo; // Useful to generate structs
};


#endif /* FIELDBLOCK_HPP_ */
