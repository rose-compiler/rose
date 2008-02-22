#ifndef COMMAND_OPTIONS_H
#define COMMAND_OPTIONS_H
#include <string>
#include <vector>
#include <iostream>
//Liao, 2/22/2008, fix the undefined STD in projects/arrayOptimization/ArrayAnnot.h
#define STD std::
class CmdOptions
{
  static CmdOptions *inst;
 public:
  std::vector<std::string> opts; // So its .end() method is accessible

  CmdOptions() : opts() {}
  void SetOptions  (const std::vector<std::string>& opts);
  void SetOptions  (int argc, const char* argv[] );
  void SetOptions  (int argc, char* argv[] );
  bool HasOption( const std::string& opt); 
  std::vector<std::string>::const_iterator GetOptionPosition( const std::string& opt); 
  static CmdOptions* GetInstance();
};

#endif
