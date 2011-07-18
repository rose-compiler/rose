/*
 * transformWorklist.h
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#ifndef TRANSFORMATIONWORKLIST_H_
#define TRANSFORMATIONWORKLIST_H_

#include "transformationOptions.h"

class Transformation
{
public:
	Transformation(TransformationOptions::LayoutType _option, vector<string> _input, vector<string> _output);
	bool containsInput(string name);
	int getInputIndex( string name );
	TransformationOptions::LayoutType getOption();
	vector<string> getInput();
	vector<string> getOutput();

private:
	TransformationOptions::LayoutType option;
	vector<string> input;
	vector<string> output;
};

class TransformationWorklist
{
public:
	TransformationWorklist();
	void addToWorklist (TransformationOptions::LayoutType option, vector<string> input, vector<string> output);
	Transformation getTransformation( int index );
	vector<Transformation> getWorklist();

private:
	vector<Transformation> worklist;

};

#endif /* TRANSFORMATIONWORKLIST_H_ */
