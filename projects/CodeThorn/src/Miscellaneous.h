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
#include "CommandLineOptions.h"

using namespace std;

namespace CodeThorn {

void write_file(std::string filename, std::string data);
string int_to_string(int x);
string color(string name);

 template<typename ValueType>
 class Cached {
 public:
 Cached():_isValid(false) {}
 Cached(ValueType value):_isValid(true),_value(value) {}
   ValueType value() {
	 if(_isValid) {
	   return _value;
	 } else {
	   _value=update();
	   _isValid=true;
	   return _value;
	 }
   }
   void invalidate() {
	 _isValid=false;
   }
 virtual ValueType update()=0;
 private:
	 bool _isValid;
	 ValueType _value;
 };
} // end of namespace CodeThorn

#endif
