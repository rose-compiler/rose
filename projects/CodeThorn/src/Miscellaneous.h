#ifndef MISCELLANEOUS
#define MISCELLANEOUS

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <list>

namespace CodeThorn {

  void write_file(std::string filename, std::string data);
  std::string int_to_string(int x);
  std::pair<int,int> parseCsvIntPair(std::string toParse);
  std::string color(std::string name);
  void nocheck(std::string checkIdentifier, bool checkResult);
  void check(std::string checkIdentifier, bool checkResult, bool check=true);
  std::string readableruntime(double timeInMilliSeconds);
  long getPhysicalMemorySize();
} // end of namespace CodeThorn

#endif
