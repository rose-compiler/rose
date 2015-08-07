#ifndef MATLAB_PARSER_H
#define MATLAB_PARSER_H

#include "sage3basic.h"

#include <string>


class MatlabParser
{
 public:

  static SgProject* frontend(int argc, char* argv[]);
};

#endif
