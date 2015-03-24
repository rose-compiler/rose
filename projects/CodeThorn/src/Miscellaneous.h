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
  std::string replace_string(std::string toModify, std::string toReplace, std::string with);
  void nocheck(std::string checkIdentifier, bool checkResult);
  void check(std::string checkIdentifier, bool checkResult, bool check=true);

 namespace Parse {
   bool checkWord(std::string w,std::istream& is);
   void parseString(std::string w,std::istream& is);
   bool integer(std::istream& is, int& num);
   int spaces(std::istream& is);
   int whitespaces(std::istream& is);
   std::set<int> integerSet(std::string intSet);
   std::list<int> integerList(std::string intList);
 }

} // end of namespace CodeThorn

#endif
