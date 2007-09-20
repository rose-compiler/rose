/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#include "PrologInt.h"
#include <sstream>

std::string PrologInt::getRepresentation() {
  std::stringstream ss;
  std::string s;
  ss << mValue;
  ss >> s;
  return s;
}
