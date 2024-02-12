#include <Rose/StringUtility/Escape.h>

#include <Rose/StringUtility/StringToNumber.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace Rose {
namespace StringUtility {

std::string
htmlEscape(const std::string& s) {
    std::string s2;
    for (size_t i = 0; i < s.size(); ++i) {
        switch (s[i]) {
            case '<': s2 += "&lt;"; break;
            case '>': s2 += "&gt;"; break;
            case '&': s2 += "&amp;"; break;
            default: s2 += s[i]; break;
        }
    }
    return s2;
}

std::string
cEscape(char ch, char context) {
    std::string result;
    switch (ch) {
        case '\a':
            result += "\\a";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\t':
            result += "\\t";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\v':
            result += "\\v";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\"':
            if ('"' == context) {
                result += "\\\"";
            } else {
                result += ch;
            }
            break;
        case '\'':
            if ('\'' == context) {
                result += "\\'";
            } else {
                result += ch;
            }
            break;
        case '\\':
            result += "\\\\";
            break;
        default:
            if (isprint(ch)) {
                result += ch;
            } else {
                char buf[8];
                snprintf(buf, sizeof(buf), "\\%03o", (unsigned)(unsigned char)ch);
                result += buf;
            }
            break;
    }
    return result;
}

std::string
cEscape(const std::string &s, char context) {
    std::string result;
    for (char ch: s)
        result += cEscape(ch, context);
    return result;
}

std::string
cUnescape(const std::string &s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if ('\\' == s[i] && i+1 < s.size()) {
            ++i;
            switch (s[i]) {
                case '?':
                    result += '?';
                    break;
                case '"':
                    result += '"';
                    break;
                case '\'':
                    result += '\'';
                    break;
                case 'a':
                    result += '\a';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'u':
                    result += "\\u";                    // Unicode is not supported; leave it escaped
                    break;
                case 'U':
                    result += "\\U";                    // Unicode is not supported; leave it escaped
                    break;
                case 'v':
                    result += '\v';
                    break;
                case 'x': {
                    unsigned byte = 0;
                    while (i+1 < s.size() && isxdigit(s[i+1]))
                        byte = ((16*byte) + hexadecimalToInt(s[++i])) & 0xff;
                    result += (char)byte;
                    break;
                }
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7': {
                    unsigned byte = 0, nchars = 0;
                    --i;
                    while (i+1 < s.size() && nchars++ < 3 && strchr("01234567", s[i+1]))
                        byte = 8*byte + s[++i] - '0';
                    result += (char)byte;
                    break;
                }
                default:
                    result += std::string("\\") + s[i];
            }
        } else {
            result += s[i];
        }
    }
    return result;
}

// Escaping special characters in shells is difficult. There are many classes of characters:
//
//   1. Characters that can appear unescaped and outside quotes (bare). They can also appear unescaped inside single or
//      double quotes.  Most characters fall into this category.
//
//   2. Characters that must be escaped when outside quotes or (not escaped) inside double or single quotes.  These are
//      characters like "*", "?", "=", "{", "}", "[", "]", etc. and consist of most printable non-alphanumeric characters.
//
//   3. Characters that cannot be escaped by some shells. These are things like non-printable control characters
//      and characters above 0177. The Bash shell has a special syntax for escaping these in a C-like manner.
std::string
bourneEscape(const std::string &s) {
    // If the string is empty it needs to be quoted.
    if (s.empty())
        return "''";

    // The presence of non-printing characters or single quotes trumps all others and requires C-style quoting
    for (char ch: s) {
        if (!::isprint(ch) || '\'' == ch)
            return "$'" + cEscape(s, '\'') + "'";
    }

    // If the string contains any shell meta characters or white space that must be quoted then single-quote the entire string
    // and escape backslashes.
    for (char ch: s) {
        if (!::isalnum(ch) && !strchr("_-+./=", ch))
            return "'" + boost::replace_all_copy(s, "\\", "\\\\") + "'";
    }

    // No quoting or escaping necessary
    return s;
}

std::string
yamlEscape(const std::string &s) {
    if (s.empty())
        return "\"\"";

    const std::string escaped = cEscape(s);
    if (s != escaped)
        return "\"" + escaped + "\"";

    if (!std::isalnum(s[0]) || !std::isalnum(s[s.size()-1]))
        return "\"" + s + "\"";
    if (boost::contains(s, "  "))
        return "\"" + s + "\"";

    if (s.find(':') != std::string::npos)
        return "\"" + s + "\"";

    const std::string lc = boost::to_lower_copy(s);
    if ("yes" == lc || "true" == lc || "no" == lc || "false" == lc)
        return"\"" + escaped + "\"";

    return s;
}

std::string
jsonEscape(const std::string &s) {
    return cEscape(s);
}

std::string
csvEscape(const std::string &s) {
    const std::string quote = s.find_first_of(",\r\n\"") == std::string::npos ? "" : "\"";
    return quote + boost::replace_all_copy(s, "\"", "\"\"") + quote;
}

// DQ (12/8/2016): This version fixed most of the issues that Robb raised with the escapeNewLineCharaters() function.
// Unaddressed is: point #4. It escapes newlines using "l" rather than the more customary "n".
// I have implemented this to support the DOT graphics where it is used.  This can be a subject
// for discussion at some point a bit later (it is late evening on the west coast presently).
std::string
escapeNewlineAndDoubleQuoteCharacters ( const std::string & X )
   {
     std::string returnString;
     int stringLength = X.length();

     for (int i=0; i < stringLength; i++)
        {
          if ( X[i] == '\n' )
             {
               returnString += "\\l";
             }
          else
             {
               if ( X[i] == '\"' )
                  {
                    returnString += "\\\"";
                  }
               else
                  {
                    returnString += X[i];
                  }
             }
        }

     return returnString;
   }



} // namespace
} // namespace
