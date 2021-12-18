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

  extern bool colorsEnabled; // only accessed by color() function

  // if writing of file is successful, true is returned.
  bool write_file(std::string filename, std::string data);
  std::string int_to_string(int x);
  std::pair<int,int> parseCsvIntPair(std::string toParse);
  std::string color(std::string name);
  void nocheck(std::string checkIdentifier, bool checkResult);
  void check(std::string checkIdentifier, bool checkResult, bool check=true);
  std::string readableRunTimeFromSeconds(size_t timeInSeconds);
  std::string readableRunTimeFromMilliSeconds(double timeInMilliSeconds);
  long getPhysicalMemorySize();

  // obtains CodeThorn library version number and checks with function checkVersionNumber
  bool checkCodeThornLibraryVersionNumber(const std::string &need);
  // obtains ROSE version number from ROSE_PACKAGE_VERSION and checks with function checkVersionNumber
  bool checkRoseVersionNumber(const std::string &need);
    // returns true if need<=have; supported format: num(.num)*
  bool checkVersionNumber(const std::string &need, const std::string &have);

} // end of namespace CodeThorn

#endif
