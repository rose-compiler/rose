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
#include <algorithm>

// DQ (3/24/2018): Added include file (using Boost 1.60, if that makes a difference).
#include <vector>

namespace CodeThorn {
  std::string replace_string(std::string toModify, std::string toReplace, std::string with);
  int randomIntInRange(std::pair<int,int> range);
  std::list<int> nDifferentRandomIntsInRange(int n, std::pair<int,int> range);
  std::list<int> nDifferentRandomIntsInSet(int n, std::set<int> values);

  // schroder3 (2016-08-16): Wrapper around std::set_intersection that allows a simple inline usage.
  template <typename T>
  std::set<T> setIntersect(std::set<T> a, std::set<T> b) {
    std::set<T> result;
    std::set_intersection(a.begin(), a.end(),
                          b.begin(), b.end(),
                          std::inserter(result, result.begin()));
    return result;
  }
 namespace Parse {
   bool checkWord(std::string w,std::istream& is);
   void parseString(std::string w,std::istream& is);
   bool integer(std::istream& is, int& num);
   int spaces(std::istream& is);
   int whitespaces(std::istream& is);
   std::set<int> integerSet(std::string intSet);
   std::list<int> integerList(std::string intList);
   std::list<std::set<int> > integerSetList(std::string intSetList);
   std::vector<std::string> commandLineArgs(std::string commandLine);
 }

} // end of namespace CodeThorn

#endif
