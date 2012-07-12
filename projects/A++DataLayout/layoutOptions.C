#include <string>
#include "layoutOptions.h"

using namespace std;

struct OptionsStruct {
	LayoutOptions::LayoutType variant;
	string name;
} layoutOptionNames[5] = {
		{ LayoutOptions::UnknownTransformation, "UnknownTransformationOption" },
		{ LayoutOptions::InterleaveAcrossArrays, "interleaveAcrossArrays" }, };

string LayoutOptions::getOptionString(int i) {
	return layoutOptionNames[i].name;
}

LayoutOptions::LayoutType LayoutOptions::getLayoutType(string name) {

	if (name.compare("interleaveAcrossArrays") == 0)
		return LayoutOptions::InterleaveAcrossArrays;

	return LayoutOptions::UnknownTransformation;
}
