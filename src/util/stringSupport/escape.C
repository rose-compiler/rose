#include "escape.h"
#include <string>
#include <sstream>
#include <iomanip>

std::string escapeString(const std::string & s) {
  std::string result;
  for (unsigned int i = 0; i < s.length(); ++i) {
    switch (s[i]) {
      case '\\': 
        result += "\\\\";
        break;
      case '"':
        result += "\\\"";
        break;
      case '\a':
        result += "\\a";
        break;
      case '\f':
        result += "\\f";
        break;
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '\v':
        result += "\\v";
        break;
      default:
        if (isprint(s[i])) {
          result.push_back(s[i]);
        } else {
          std::ostringstream stream;
          stream << '\\';
          stream << std::setw(3) << std::setfill('0') << (unsigned int)(s[i]);
          result += stream.str();
        }
        break;
    }
  }
  return result;
}
