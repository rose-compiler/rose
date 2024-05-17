#ifndef COMMAND_OPTIONS_H
#define COMMAND_OPTIONS_H
#include <string>
#include <vector>
#include <iostream>
#include "rosedll.h"

class ROSE_UTIL_API CmdOptions
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

class DebugLog {
  int r = 0;
  std::string what_to_debug_;
 public:
   DebugLog(const std::string& what_to_debug) : what_to_debug_(what_to_debug) {}

   bool operator()() {
    if (r == 0) {
      if (CmdOptions::GetInstance()->HasOption(what_to_debug_))
         r = 1;
      else
         r = -1;
    }
    return r ==1;
   }
   bool operator()(const std::string& to_print) {
    if (operator()()) {
      std::cerr << to_print << "\n";
      return true;
    }
    return false;
   }
};

#endif
