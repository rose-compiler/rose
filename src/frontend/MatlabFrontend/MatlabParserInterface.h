#ifndef MATLAB_PARSER_INTERFACE_H
#define MATLAB_PARSER_INTERFACE_H

#include "sage3basic.h"

#include <string>


class MatlabParserInterface
{
 public:

  static SgProject* Parse(int argc, char* argv[]);
};

#endif
