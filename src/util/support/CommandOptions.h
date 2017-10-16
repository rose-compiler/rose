#ifndef COMMAND_OPTIONS_H
#define COMMAND_OPTIONS_H
#include <string>
#include <vector>
#include <iostream>

class CmdOptions
{
  static CmdOptions *inst;
  std::vector<std::string> opts; 

 public:
  CmdOptions() : opts() {}
  void SetOptions  (const std::vector<std::string>& opts);
  void SetOptions  (int argc, const char* argv[] );
  void SetOptions  (int argc, char* argv[] );
  bool HasOption( const std::string& opt); 
  std::vector<std::string>::const_iterator GetOptionPosition( const std::string& opt); 
  std::vector<std::string>::const_iterator begin() { return opts.begin(); }
  std::vector<std::string>::const_iterator end() { return opts.end(); }
  static CmdOptions* GetInstance();
};

#endif
