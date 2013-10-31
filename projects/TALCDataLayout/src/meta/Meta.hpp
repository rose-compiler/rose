/*
 * Meta.hpp
 *
 *  Created on: Oct 11, 2012
 *      Author: Kamal Sharma
 */

#ifndef META_HPP_
#define META_HPP_

#include <map>

#include "Common.hpp"
#include "View.hpp"
#include "Field.hpp"

class Meta
{
public:
	Meta() {};
	void addView(View *view);
	void buildFieldMap();
	Field* getField(string name);
	void print();

private:
	void traverseView(View *view);
	vector<View*> viewList;
	// This is useful for efficiently accessing
	// fields inside a meta
	map<string, Field*> fieldMap;
};


#endif /* META_HPP_ */
