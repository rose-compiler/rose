#include "escape.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

std::string escapeString(const std::string & s) {
  std::string result;
  for (size_t i = 0; i < s.length(); ++i) {
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
          stream << std::setw(3) << std::setfill('0') <<std::oct << (unsigned)(unsigned char)(s[i]);
          result += stream.str();
        }
        break;
    }
  }
  return result;
}

std::string unescapeString(const std::string& s) {
  std::string result;
  for (size_t i = 0; i < s.length(); ++i) {
    // Body of this loop can change i
    if (s[i] == '\\') {
      assert (i + 1 < s.length());
      switch (s[i + 1]) {
        case 'a': result += '\a'; break;
        case 'e': result += '\033'; break;
        case 'f': result += '\f'; break;
        case 'n': result += '\n'; break;
        case 'r': result += '\r'; break;
        case 't': result += '\t'; break;
        case 'v': result += '\v'; break;
        case '0':
        case '1':
        case '2':
        case '3': {
          ++i;
          if (s[i] == '0' && i + 1 == s.length()) {
            result += '\0';
            break;
          }
          assert (i + 2 < s.length());
          unsigned char c = (s[i] - '0') << 6;
          ++i;
          assert (s[i] >= '0' && s[i] <= '7');
          c |= (s[i] - '0') << 3;
          ++i;
          assert (s[i] >= '0' && s[i] <= '7');
          c |= (s[i] - '0');
          break;
        }
        default: ++i; result += s[i]; break;
      }
    } else if (s[i] == '"') {
      return result;
    } else {
      result += s[i];
    }
  }
  return result;
}
