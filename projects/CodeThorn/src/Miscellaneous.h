#ifndef MISCELLANEOUS
#define MISCELLANEOUS

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
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
  std::string color(std::string name);
  void nocheck(std::string checkIdentifier, bool checkResult);
  void check(std::string checkIdentifier, bool checkResult, bool check=true);

} // end of namespace CodeThorn

#endif
