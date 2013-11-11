/*
 * meta.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: Kamal Sharma
 */

#include "Meta.hpp"

/**
 * Adds a view to Meta
 * @param view
 */
void Meta::addView(View *view) {
	viewList.push_back(view);
}

/**
 * Internal method to traverse a view
 * @param view
 */
void Meta::traverseView(View *view) {

	pair<map<string, Field*>::iterator, bool> ret;

	vector<FieldBlock*> fieldBlockList = view->getFieldBlockList();
	foreach(FieldBlock * fieldBlock, fieldBlockList)
	{
		vector<Field*> fieldList = fieldBlock->getFieldList();
		foreach(Field * field, fieldList)
		{
			ret = fieldMap.insert(
					pair<string, Field*>(field->getName(), field));
			if (ret.second == false) {
				traceerror<< "Field : " << field->getName() << " already exist" << endl;
				traceerror << " Check you meta file to see if duplicate entries exist." << endl;
				exit(1);
			}
		}
	}
	vector<View*> subViewList = view->getSubViewList();
	foreach(View * subView, subViewList)
	{
		traverseView(subView);
	}
}

/**
* Builds the fieldMap for efficient accesses
* to field within this meta
*/
void Meta::buildFieldMap() {

	foreach(View * view, viewList)
	{
		traverseView(view);
	}
}

/**
 * Fetches a field from built-up FieldMap
 * @param name Field Name
 * @return Field Pointer
 */
Field* Meta::getField(string name) {
	map<string, Field*>::iterator it;
	it = fieldMap.find(name);

	if (it == fieldMap.end()) {
		return NULL;
	} else {
		return (*it).second;
	}
}

/**
 * Prints Meta by printing its view
 */
void Meta::print() {
	foreach(View * view, viewList)
	{
		view->print();
	}
}

