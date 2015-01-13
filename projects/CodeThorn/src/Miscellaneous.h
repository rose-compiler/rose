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

using namespace std;

namespace CodeThorn {

  void write_file(std::string filename, std::string data);
  string int_to_string(int x);
  string color(string name);
  string replace_string(string toModify, string toReplace, string with);
  void nocheck(string checkIdentifier, bool checkResult);
  void check(string checkIdentifier, bool checkResult, bool check=true);

 namespace Parse {
   bool checkWord(string w,istream& is);
   void parseString(string w,istream& is);
   bool integer(istream& is, int& num);
   int spaces(istream& is);
   int whitespaces(istream& is);
   set<int> integerSet(string intSet);
   list<int> integerList(string intList);
 }

} // end of namespace CodeThorn

#endif
