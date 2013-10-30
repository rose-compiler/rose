/*
 * PrintUtil.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: Kamal Sharma
 */

#include "PrintUtil.hpp"

void PrintUtil::printEquals()
{
	for(int i=0; i<8 ; i++)
		trace << "=";
}

void PrintUtil::printDash()
{
	for(int i=0; i<8 ; i++)
		trace << "-";
}

void PrintUtil::printStart(string text)
{
	printEquals();
	trace << text << " Start ";
	printEquals();
	trace << endl;
}

void PrintUtil::printEnd(string text)
{
	printEquals();
	trace << " " << text << " End ";
	printEquals();
	trace << endl;
}

void PrintUtil::printHeader(char *text)
{
	string str(text);
	printHeader(str);
}

void PrintUtil::printHeader(string text)
{
	printDash();
	trace << " " << text << " ";
	printDash();
	trace << endl;
}

void PrintUtil::printFunctionStart(string text)
{
	text += " Start ";
	printHeader(text);
}

void PrintUtil::printFunctionEnd(string text)
{
	text += " End ";
	printHeader(text);
}
