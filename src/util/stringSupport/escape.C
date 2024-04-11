#include "escape.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

std::string escapeString(const std::string & s) {

// DQ (2/4/2014): Note that this function can not be used on the asm strings
// for the unparser.  When it can frequently work, the present of "\n" and "\t"
// substrings causes the transformation to "\\n" and \\t" which is a bug when
// used with the asmembler.  So a special version of this function that does
// not process "\n" to "\\n" is defined in the unparser.  Note that we still
// need to process '\n' to "\n" and this behavior is still preserved.
// Test codes demonstrating this are in test2014_83-87.c.

// See also, Rose::StringUtility::cEscape.

  std::string result;
  for (size_t i = 0; i < s.length(); ++i) 
  {
    switch (s[i]) {
      case '\\': 
#if 0
        printf ("In escapeString(): processing \\\\ character \n");
#endif
        result += "\\\\";
        break;
      case '"':
#if 0
        printf ("In escapeString(): processing \\\" character \n");
#endif
        result += "\\\"";
        break;
      case '\a':
#if 0
        printf ("In escapeString(): processing \\a character \n");
#endif
        result += "\\a";
        break;
      case '\f':
#if 0
        printf ("In escapeString(): processing \\f character \n");
#endif
        result += "\\f";
        break;
      case '\n':
#if 0
        printf ("In escapeString(): processing \\n character \n");
#endif
        result += "\\n";
        break;
      case '\r':
#if 0
        printf ("In escapeString(): processing \\r character \n");
#endif
        result += "\\r";
        break;
      case '\t':
#if 0
        printf ("In escapeString(): processing \\t character \n");
#endif
        result += "\\t";
        break;
      case '\v':
#if 0
        printf ("In escapeString(): processing \\v character \n");
#endif
        result += "\\v";
        break;
      default:
#if 0
        printf ("In escapeString(): processing default case character \n");
#endif
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

// See also, Rose::StringUtility::cUnescape
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
