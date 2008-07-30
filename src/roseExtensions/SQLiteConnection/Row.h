#ifndef ROW_H
#define ROW_H

#include <vector>
#include <iostream>
#include <ColData.h>

class Row
{
 public:
   ColData operator[] (int i) { return rows[i]; }
   int size () { return rows.size(); }
   bool empty () { return rows.empty(); }
   void reserve (int i) { rows.reserve(i); }
   void push_back(const std::string& s) { rows.push_back(s); }
 private:
   std::vector <std::string> rows;
};

#endif
