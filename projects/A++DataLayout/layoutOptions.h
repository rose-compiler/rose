/*
 * transformationOption.h
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#ifndef LAYOUTOPTIONS_H_
#define LAYOUTOPTIONS_H_

#include <string>


class LayoutOptions
{
public:
	enum LayoutType
	{
		UnknownTransformation = 0,
		InterleaveAcrossArrays = 1,
	};

	static std::string getOptionString ( int i );
	static LayoutType getLayoutType( std::string name);
};

#endif /* LAYOUTOPTIONS_H_ */
