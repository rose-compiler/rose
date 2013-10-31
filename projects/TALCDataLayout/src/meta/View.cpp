/*
 * View.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: Kamal Sharma
 */
#include "View.hpp"

/**
 * Constructor
 * @param name View name
 */
View::View(string name) : parentView(NULL)
{
	this->name = name;
}

/**
 * Inserts a view inside a parent View
 * @param subView child View
 */
void View::addSubView(View *subView)
{
	subView->setParent(this);
	subViewList.push_back(subView);
}

/**
 * Inserts a Field Block in the current View
 * @param fieldBlock
 */
void View::addFieldBlock(FieldBlock *fieldBlock)
{
	fieldBlock->setView(this);
	fieldBlockList.push_back(fieldBlock);
}

/**
 * Get the View name
 * @return View Name
 */
string View::getViewName(){
	return name;
}

/**
 * Get the Field Block within a View
 * @return Field Block Vector
 */
vector<FieldBlock*> View::getFieldBlockList()
{
	return fieldBlockList;
}

/**
 * Get the Sub View List from a view
 * @return Sub View vector
 */
vector<View*> View::getSubViewList()
{
	return subViewList;
}

/**
 * Sets the parent View of current View
 * This is only necessary for nested Views
 * @param view Parent View
 */
void View::setParent(View *view){
	parentView = view;
}

/**
 * Prints a View and its subView
 */
void View::print(){
	trace << "View: " << name << std::endl;

	foreach(View *view, subViewList)
	{
		view->print();
	}

	foreach(FieldBlock *fieldBlock, fieldBlockList)
	{
		fieldBlock->print();
	}
}
