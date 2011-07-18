/*
 * transformationOption.h
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#ifndef TRANSFORMATIONOPTIONS_H_
#define TRANSFORMATIONOPTIONS_H_

#include <string>

using namespace std;

class TransformationOptions
{
public:
	enum LayoutType
	{
		UnknownTransformation = 0,
		InterleaveAcrossArrays = 1,
	};

	static std::string getOptionString ( int i );
	static LayoutType getLayoutType( string name);
};

#endif /* TRANSFORMATIONOPTIONS_H_ */
