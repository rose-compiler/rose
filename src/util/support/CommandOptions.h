#ifndef COMMAND_OPTIONS_H
#define COMMAND_OPTIONS_H
#include <stdlib.h>
#include <string>
#include <iostream>

#define STD std::

class CmdOptions
{
  STD string cmd;
  static CmdOptions *inst;
 public:
  CmdOptions() : cmd("") {}
  void SetOptions  (int argc, char* argv[]);
  const char* HasOption( const STD string& opt); 
  static CmdOptions* GetInstance();
};

#endif
