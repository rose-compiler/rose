#include <string>
#include "transformationOptions.h"

using namespace std;

struct OptionsStruct
   {
     TransformationOptions::LayoutType variant;
     string name;
   } transformationOptionNames[5] = {
          {TransformationOptions::UnknownTransformation,"UnknownTransformationOption"},
          {TransformationOptions::InterleaveAcrossArrays,"interleaveAcrossArrays"},
   };

string TransformationOptions::getOptionString ( int i )
{
	return transformationOptionNames[i].name;
}

TransformationOptions::LayoutType TransformationOptions::getLayoutType( string name)
{

	if(name.compare("interleaveAcrossArrays") == 0)
		return TransformationOptions::InterleaveAcrossArrays;

	return TransformationOptions::UnknownTransformation;
}
