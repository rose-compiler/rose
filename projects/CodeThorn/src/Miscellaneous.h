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

using namespace std;

namespace CodeThorn {

  void write_file(std::string filename, std::string data);
  string int_to_string(int x);
  string color(string name);

 namespace Parse {
   bool checkWord(string w,istream& is);
   void parseString(string w,istream& is);
   bool integer(istream& is, int& num);
   int spaces(istream& is);
   int whitespaces(istream& is);
   set<int> integerSet(string intSet);
 }

} // end of namespace CodeThorn

#endif
