/*
 * Parser.hpp
 *
 *  Created on: Oct 15, 2012
 *      Author: Kamal Sharma
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

//#include "rose.h"

#include "Common.hpp"
#include "Meta.hpp"
#include "View.hpp"
#include "FieldBlock.hpp"
#include "Field.hpp"

using namespace std;

class Parser
{
public:
	Parser(string filename);
	//Parser(Rose_STL_Container<string> &args);
	Meta* parseFile();

private:
	string filename;
};



#endif /* PARSER_HPP_ */
