
#include "transformationWorklist.h"
#include "rose.h"

using namespace std;

Transformation::Transformation(LayoutOptions::LayoutType _option, vector<string> _input,
		vector<string> _output) {
	ROSE_ASSERT(_option != LayoutOptions::UnknownTransformation);
	option = _option;
	input = _input;
	output = _output;
}

LayoutOptions::LayoutType Transformation::getOption() {
	return option;
}

vector<string> Transformation::getInput() {
	return input;
}

vector<string> Transformation::getOutput() {
	return output;
}

bool Transformation::containsInput(string name) {
	for (vector<string>::iterator iter = input.begin(); iter != input.end(); iter++) {
		string curName = *iter;
		if (curName.compare(name) == 0)
			return true;
	}
	return false;
}

int Transformation::getInputIndex(string name) {
	int count = 0;
	for (vector<string>::iterator iter = input.begin(); iter != input.end(); iter++, count++) {
		string curName = *iter;
		if (curName.compare(name) == 0)
			return count;
	}
	return -1;
}



TransformationWorklist::TransformationWorklist() {

}

void TransformationWorklist::addToWorklist(LayoutOptions::LayoutType option,
		vector<string> input, vector<string> output) {

	Transformation* t = new Transformation(option, input, output);
	worklist.push_back(*t);
}

vector<Transformation> TransformationWorklist::getWorklist() {
	return worklist;
}

Transformation TransformationWorklist::getTransformation(int index) {

	if (index > worklist.size())
		ROSE_ABORT();

	return worklist.at(index);
}


int TransformationWorklist::size()
{
	return worklist.size();
}
