#ifndef COMMAND_OPTIONS_H
#define COMMAND_OPTIONS_H
#include <string>
#include <vector>
#include <iostream>
//! Old class for command option handling. 
//Please use the better src/util/commandlineProcessing/commandline_processing.h instead
class CmdOptions
{
  static CmdOptions *inst;
 public:
  std::vector<std::string> opts; // So its .end() method is accessible
  // This is modified by some code to prevent ROSE trying to parse loop transformation options

  CmdOptions() : opts() {}
  void SetOptions  (const std::vector<std::string>& opts);
  void SetOptions  (int argc, const char* argv[] );
  void SetOptions  (int argc, char* argv[] );
  bool HasOption( const std::string& opt); 
  std::vector<std::string>::const_iterator GetOptionPosition( const std::string& opt); 
  static CmdOptions* GetInstance();
};

#endif
