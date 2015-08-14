#ifndef MATLAB_PARSER_H
#define MATLAB_PARSER_H

class MatlabParser
{
 public:

  static SgProject* frontend(int argc, char* argv[]);
};

#endif
