#ifndef MISCELLANEOUS
#define MISCELLANEOUS

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "CommandLineOptions.h"

using namespace std;

void write_file(std::string filename, std::string data);
string int_to_string(int x);
string color(string name);

#endif
