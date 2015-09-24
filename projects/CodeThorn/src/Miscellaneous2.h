#ifndef MISCELLANEOUS2
#define MISCELLANEOUS2

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

namespace SPRAY {
  std::string replace_string(std::string toModify, std::string toReplace, std::string with);
 namespace Parse {
   bool checkWord(std::string w,std::istream& is);
   void parseString(std::string w,std::istream& is);
   bool integer(std::istream& is, int& num);
   int spaces(std::istream& is);
   int whitespaces(std::istream& is);
   std::set<int> integerSet(std::string intSet);
   std::list<int> integerList(std::string intList);
 }

} // end of namespace SPRAY

#endif
