/*
 * View.h
 *
 *  Created on: Oct 11, 2012
 *      Author: kamalsharma
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <string>
#include <vector>

#include "Common.hpp"
#include "FieldBlock.hpp"

using namespace std;

class View
{
public:
	View() : parentView(NULL) {};
	View(string name);

	void addSubView(View *view);
	void addFieldBlock(FieldBlock *fieldBlock);

	// Setters
	void setParent(View *view);

	// Getters
	string getViewName();
	vector<FieldBlock*> getFieldBlockList();
	vector<View*> getSubViewList();

	// Print
	void print();

private:
	vector<View*> subViewList;
	vector<FieldBlock*> fieldBlockList;
	string name;
	View *parentView;
};

#endif /* VIEW_H_ */
