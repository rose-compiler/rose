/*
 * PrintUtil.hpp
 *
 *  Created on: Oct 18, 2012
 *      Author: Kamal Sharma
 */

#ifndef PRINTUTIL_HPP_
#define PRINTUTIL_HPP_

#include "Common.hpp"
#include <string>

using namespace std;

class PrintUtil
{
public:
	PrintUtil() {};
	static void printStart(string text);
	static void printEnd(string text);
	static void printHeader(string text);
	static void printHeader(char *text);
	static void printFunctionStart(string text);
	static void printFunctionEnd(string text);
private:
	static void printEquals();
	static void printDash();
};


#endif /* PRINTUTIL_HPP_ */
