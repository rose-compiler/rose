/*
 * transformWorklist.h
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#ifndef TRANSFORMATIONWORKLIST_H_
#define TRANSFORMATIONWORKLIST_H_

#include <iostream>
#include <string>
#include <vector>

#include "layoutOptions.h"

using namespace std;

class Transformation
{
public:
	Transformation(LayoutOptions::LayoutType _option, vector<string> _input, vector<string> _output);
	bool containsInput(string name);
	int getInputIndex( string name );
	LayoutOptions::LayoutType getOption();
	vector<string> getInput();
	vector<string> getOutput();

private:
	LayoutOptions::LayoutType option;
	vector<string> input;
	vector<string> output;
};

class TransformationWorklist
{
public:
	TransformationWorklist();
	void addToWorklist (LayoutOptions::LayoutType option, vector<string> input, vector<string> output);
	Transformation getTransformation( int index );
	vector<Transformation> getWorklist();
	int size();

private:
	vector<Transformation> worklist;

};

#endif /* TRANSFORMATIONWORKLIST_H_ */
