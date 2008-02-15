#include "caselessCompare.h"
#include <ctype.h>
#include <algorithm>

bool
CaseLessCompare::caseInsensitive(const char c1, const char c2)
{
  return toupper(c1) < toupper(c2);
}

bool 
CaseLessCompare::operator() (const std::string &s1, const std::string &s2) const {
  return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), CaseLessCompare::caseInsensitive);
}
