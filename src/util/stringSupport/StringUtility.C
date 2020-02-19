#define __STDC_FORMAT_MACROS

// ROSE includes
#include <StringUtility.h>
#include <FileUtility.h>

#include <rose_config.h>
#include <integerOps.h>
#include "rose_msvc.h"                                  // DQ (3/22/2009): Added MSVS support for ROSE.

// Other includes
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <inttypes.h>
#include <iostream>
#include <sstream>

// DQ (1/21/2010): Use this to turn off the use of #line in ROSETTA generated code.
#define SKIP_HASH_LINE_NUMBER_DECLARATIONS_IN_GENERATED_FILES

namespace Rose {
namespace StringUtility {




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Character-escaping functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
                sprintf(buf, "\\%03o", (unsigned)(unsigned char)ch);
                result += buf;
            }
            break;
    }
    return result;
}

std::string
cEscape(const std::string &s, char context) {
    std::string result;
    BOOST_FOREACH (char ch, s)
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
    // The presence of non-printing characters trumps all others and requires C-style quoting
    BOOST_FOREACH (char ch, s) {
        if (!::isprint(ch))
            return "$'" + cEscape(s, '\'') + "'";
    }

    // If the string contains any shell meta characters that must be quoted then single-quote the entire string and
    // handle additional single quotes and backslashes specially.
    BOOST_FOREACH (char ch, s) {
        if (!::isalnum(ch) && s.find_first_of("_-+./") == std::string::npos)
            return "'" + boost::replace_all_copy(boost::replace_all_copy(s, "\\", "\\\\"), "'", "'\"'\"'");
    }

    // No quoting or escaping necessary
    return s;
}

// [Robb P Matzke 2016-06-15]: deprecated
std::string
escapeNewLineCharaters ( const std::string & X )
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Splitting and joining strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string>
split(char separator, const std::string &str, size_t maxparts, bool trim_white_space) {
    return split(std::string(1, separator), str, maxparts, trim_white_space);
}

std::vector<std::string>
split(const std::string &separator, const std::string &str, size_t maxparts, bool trim_white_space) {
    std::vector<std::string> retval;
    if (0==maxparts || str.empty())
        return retval;
    if (separator.empty()) {
        for (size_t i=0; i<str.size() && i<maxparts-1; ++i)
            retval.push_back(str.substr(i, 1));
        retval.push_back(str.substr(retval.size()));
    } else {
        size_t at = 0;
        while (at<=str.size() && retval.size()+1<maxparts) {
            if (at==str.size()) {
                retval.push_back("");                   // string ends with separator
                break;
            }
            size_t sep_at = str.find(separator, at);
            if (sep_at==std::string::npos) {
                retval.push_back(str.substr(at));
                at = str.size() + 1;                    // "+1" means string doesn't end with separator
                break;
            } else {
                retval.push_back(str.substr(at, sep_at-at));
                at = sep_at + separator.size();
            }
        }
        if (at<str.size() && retval.size()<maxparts)
            retval.push_back(str.substr(at));
    }

    if (trim_white_space) {
        for (size_t i=0; i<retval.size(); ++i)
            retval[i] = trim(retval[i]);
    }
    return retval;
}

std::list<std::string>
tokenize(const std::string &s, char delim) {
    std::list<std::string> l;
    std::string token;
    std::istringstream iss(s);
    while (getline(iss, token, delim))
        l.push_back(token);
    return l;
}

std::string
join(const std::string &separator, char *strings[], size_t nstrings) {
    return join_range(separator, strings, strings+nstrings);
}

std::string
join(const std::string &separator, const char *strings[], size_t nstrings) {
    return join_range(separator, strings, strings+nstrings);
}

std::string
join(char separator, char *strings[], size_t nstrings) {
    return join_range(std::string(1, separator), strings, strings+nstrings);
}

std::string
join(char separator, const char *strings[], size_t nstrings) {
    return join_range(std::string(1, separator), strings, strings+nstrings);
}

std::string
joinEnglish(const std::vector<std::string> &phrases, const std::string &separator, const std::string &finalIntro) {
    if (phrases.empty())
        return "";
    if (phrases.size() == 1)
        return phrases[0];
    if (phrases.size() == 2)
        return phrases[0] + " " + finalIntro + " " + phrases[1];

    std::string s;
    for (size_t i=0; i<phrases.size()-1; ++i)
        s += phrases[i] + separator + " ";
    return s + finalIntro + " " + phrases.back();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Container versus scalar functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
listToString(const std::list<std::string> &container, bool separateStrings) {
    std::string separator = separateStrings ? " \n" : " ";
    std::string retval = join_range(separator, container.begin(), container.end());
    if (!container.empty())
        retval += separator;
    return retval;
}

std::string
listToString(const std::vector<std::string> &container, bool separateStrings) {
    std::string separator = separateStrings ? " \n" : " ";
    std::string retval = join_range(separator, container.begin(), container.end());
    if (!container.empty())
        retval += separator;
    return retval;
}

std::string
listToString(const std::list<int> &container, bool separateStrings) {
    std::string separator = separateStrings ? " \n" : " ";
    std::string retval = join_range(separator, container.begin(), container.end());
    if (!container.empty())
        retval += separator;
    return retval;
}

std::list<std::string>
stringToList(const std::string &input) {
    std::vector<std::string> substrings = split('\n', input);
    std::list<std::string> retval(substrings.begin(), substrings.end());
    retval.remove("");
    return retval;
}

// This function was written by Bobby Philip in support of the newer approach toward handling a broader number of back-end C++
// compilers.
void
splitStringIntoStrings(const std::string& inputString, char separator, std::vector<std::string>& stringList) {
    stringList.clear();

    std::string::size_type pos = 0, lastPos = 0;
    while (true) {
        pos = inputString.find(separator, pos);
        if (pos == std::string::npos) {
            stringList.push_back(inputString.substr(lastPos));
            return;
        } else {
            if (pos != lastPos) {
                stringList.push_back(inputString.substr(lastPos, pos - lastPos));
            }
            lastPos = pos = pos + 1;
        }
    }
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for converting numbers to strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
numberToString(long long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(unsigned long long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(unsigned long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(int x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(unsigned int x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(const void* x) {
    char numberString[128];
    sprintf(numberString, "%p", x);
    return std::string(numberString);
}

std::string
numberToString(double x) {
    char numberString[128];
    sprintf(numberString, "%2.2f", x);
    return std::string(numberString);
}

#if !defined(_MSC_VER) &&                                                                                                      \
    defined(BACKEND_CXX_IS_GNU_COMPILER) &&                                                                                    \
    ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER > 6) ||                      \
     BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4) &&                                                                         \
    __WORDSIZE == 64
std::string
numberToString( __int128 x) {
    // DQ (2/22/2014): I don't think that the boost::lexical_cast can support __int128 yet.
    long long temp_x = (long long) x;
    return boost::lexical_cast<std::string>(temp_x);
}

std::string
numberToString(unsigned __int128 x) {
    // DQ (2/22/2014): I don't think that the boost::lexical_cast can support __int128 yet.
    unsigned long long temp_x = (unsigned long long) x;
    return boost::lexical_cast<std::string>(temp_x);
}
#endif

std::string
intToHex(uint64_t i) {
    std::ostringstream os;
    os << "0x" << std::hex << i;
    return os.str();
}

std::string
toHex2(uint64_t value, size_t nbits, bool show_unsigned_decimal, bool show_signed_decimal, uint64_t decimal_threshold) {
    assert(nbits>0 && nbits<=8*sizeof value);
    uint64_t sign_bit = ((uint64_t)1 << (nbits-1));

    // Hexadecimal value
    std::string retval;
    int nnibbles = (nbits+3)/4;
    char buf[64];
    snprintf(buf, sizeof buf, "0x%0*" PRIx64, nnibbles, value);
    buf[sizeof(buf)-1] = '\0';
    retval = buf;

    // unsigned decimal
    bool showed_unsigned = false;
    if (show_unsigned_decimal && value >= decimal_threshold) {
        retval = appendAsmComment(retval, numberToString(value));
        showed_unsigned = true;
    }

    // signed decimal
    if (show_signed_decimal) {
        if (0 == (value & sign_bit)) {
            // This is a positive value. Don't show it if we did already.
            if (!showed_unsigned && value >= decimal_threshold)
                retval = appendAsmComment(retval, numberToString(value));
        } else {
            // This is a negative value, so show it as negative.  We have to manually sign extend it first.
            int64_t signed_value = (value | (-1ll & ~(sign_bit-1)));
            retval = appendAsmComment(retval, numberToString(signed_value));
        }
    }
    return retval;
}

std::string
signedToHex2(uint64_t value, size_t nbits) {
    return toHex2(value, nbits, false, true);
}

std::string
unsignedToHex2(uint64_t value, size_t nbits) {
    return toHex2(value, nbits, true, false);
}

std::string
addrToString(uint64_t value, size_t nbits) {
    if (0 == nbits)
        nbits = value > 0xffffffff ? 64 : 32;
    return toHex2(value, nbits, false, false);
}

std::string
addrToString(const Sawyer::Container::Interval<uint64_t> &interval, size_t nbits) {
    if (interval.isEmpty()) {
        return "[empty]";
    } else {
        return "[" + addrToString(interval.least(), nbits) + ", " + addrToString(interval.greatest(), nbits) + "]";
    }
}

std::string
addrToString(const Sawyer::Container::IntervalSet<Sawyer::Container::Interval<uint64_t> > &iset, size_t nbits) {
    if (iset.isEmpty()) {
        return "{empty}";
    } else {
        std::string retval = "{";
        size_t i = 0;
        BOOST_FOREACH (const Sawyer::Container::Interval<uint64_t> &interval, iset.intervals()) {
            if (++i > 1)
                retval += ", ";
            retval += addrToString(interval);
        }
        retval += "}";
        return retval;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Number parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned
hexadecimalToInt(char ch) {
    if (isxdigit(ch)) {
        if (isdigit(ch))
            return ch-'0';
        if (isupper(ch))
            return ch-'A'+10;
        return ch-'a'+10;
    }
    return 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      String conversion functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
convertToLowerCase(const std::string &inputString) {
    std::string returnString = inputString;
    for (size_t i=0; i < returnString.length(); i++)
        returnString[i] = tolower(returnString[i]);
     return returnString;
}

std::string
fixLineTermination(const std::string &input) {

 // DQ (12/30/2018): This is the function that is normalizing the windows line endings in CPP directives.
 // However, eliminating the function from being called is preferable to modifing this function.
    std::string output;
    size_t nchars = input.size();
    for (size_t i=0; i<nchars; ++i) {
        if ('\r'==input[i] && i+1<nchars && '\n'==input[i+1]) {
            // CR+LF: Microsoft Windows, DEC TOPS-10, RT-11 and most other early non-Unix and non-IBM OSes, CP/M, MP/M, DOS
            // (MS-DOS, PC-DOS, etc.), Atari TOS, OS/2, Symbian OS, Palm OS.

         // DQ (12/30/2018): Detect the code that recognizes the windows line endings.
         // printf ("In fixLineTermination(): Found a windows line ending \n");

         // DQ (12/30/2018): Fix the output string to reproduce the \r\n windows line ending.
         // output += '\n';
         // output += "\r\n";
            output += '\n';

            ++i;
        } else if ('\n'==input[i] && i+1<nchars && '\r'==input[i+1]) {
            // LF+CR: Acorn BBC and RISC OS spooled text output.
            output += '\n';
            ++i;
        } else if ('\r'==input[i]) {
            // CR (only): Commodore 8-bit machines, Acorn BBC, TRS-80, Apple II family, Mac OS up to version 9 and OS-9
            output += '\n';
        } else {
            output += input[i];
        }
    }
    return output;
}

std::string
prefixLines(const std::string &lines, const std::string &prefix, bool prefixAtFront, bool prefixAtBack) {
    if (lines.empty())
        return "";

    std::string retval = prefixAtFront ? prefix : "";
    size_t at=0;
    while (at<lines.size()) {
        size_t lfpos = lines.find_first_of("\r\n", at);
        lfpos = lines.find_first_not_of("\r\n", lfpos);
        retval += lines.substr(at, lfpos-at);
        if (lfpos<lines.size())
            retval += prefix;
        at = lfpos;
    }

    if (prefixAtBack && isLineTerminated(lines))
        retval += prefix;
    return retval;
}

std::string
makeOneLine(const std::string &s, std::string replacement) {
    std::string result, spaces;
    bool eat_spaces = false;
    for (size_t i=0; i<s.size(); ++i) {
        if ('\n'==s[i] || '\r'==s[i]) {
            spaces = result.empty() ? "" : replacement;
            eat_spaces = true;
        } else if (isspace(s[i])) {
            if (!eat_spaces)
                spaces += s[i];
        } else {
            result += spaces + s[i];
            spaces = "";
            eat_spaces = false;
        }
    }
    if (!eat_spaces)
        result += spaces;
    return result;
}

std::string
trim(const std::string &str, const std::string &strip, bool at_beginning, bool at_end) {
    if (str.empty())
        return str;
    size_t first=0, last=str.size()-1;
    if (at_beginning)
        first = str.find_first_not_of(strip);
    if (at_end && first!=std::string::npos)
        last = str.find_last_not_of(strip);
    if (first==std::string::npos || last==std::string::npos)
        return "";
    assert(last>=first);
    return str.substr(first, last+1-first);
}

std::string
untab(const std::string &str, size_t tabstops, size_t colnum) {
    tabstops = std::max(tabstops, (size_t)1);
    std::string retval;
    for (size_t i=0; i<str.size(); ++i) {
        switch (str[i]) {
            case '\t': {
                size_t nspc = tabstops - colnum % tabstops;
                retval += std::string(nspc, ' ');
                colnum += nspc;
                break;
            }
            case '\n':
            case '\r':
                retval += str[i];
                colnum = 0;
                break;
            default:
                retval += str[i];
                ++colnum;
                break;
        }
    }
    return retval;
}

std::string
removeRedundantSubstrings(const std::string &s) {
    // Convert the string into a list of strings and separate out the redundant entries
    std::list<std::string> XStringList = stringToList(s);
    XStringList.sort();
    XStringList.unique();
    return listToString(XStringList);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for encoding/decoding/hashing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
encode_base64(const std::vector<uint8_t> &data, bool do_pad)
{
    return encode_base64(&data[0], data.size(), do_pad);
}

std::string
encode_base64(const uint8_t *data, size_t nbytes, bool do_pad) {
    static const char *digits = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string s;
    unsigned val = 0; // only low-order 24 bits used
    for (size_t i=0; i<nbytes; ++i) {
        switch (i%3) {
            case 0:
                val = IntegerOps::shiftLeft2<unsigned>(data[i], 16);
                break;
            case 1:
                val |= IntegerOps::shiftLeft2<unsigned>(data[i], 8);
                break;
            case 2:
                val |= data[i];
                s += digits[IntegerOps::shiftRightLogical2(val, 18) & 0x3f];
                s += digits[IntegerOps::shiftRightLogical2(val, 12) & 0x3f];
                s += digits[IntegerOps::shiftRightLogical2(val, 6) & 0x3f];
                s += digits[val & 0x3f];
                break;
        }
    }
    switch (nbytes % 3) {
        case 0:
            break;
        case 1:
            s += digits[IntegerOps::shiftRightLogical2(val, 18) & 0x3f];
            s += digits[IntegerOps::shiftRightLogical2(val, 12) & 0x3f];
            if (do_pad)
                s += "==";
            break;
        case 2:
            s += digits[IntegerOps::shiftRightLogical2(val, 18) & 0x3f];
            s += digits[IntegerOps::shiftRightLogical2(val, 12) & 0x3f];
            s += digits[IntegerOps::shiftRightLogical2(val, 6) & 0x3f];
            if (do_pad)
                s += "=";
            break;
    }
    return s;
}

std::vector<uint8_t>
decode_base64(const std::string &s) {
    std::vector<uint8_t> retval;
    retval.reserve((s.size()*3)/4+3);
    unsigned val=0, ndigits=0;
    for (size_t i=0; i<s.size(); ++i) {
        unsigned digit;
        if (s[i]>='A' && s[i]<='Z') {
            digit = s[i]-'A';
        } else if (s[i]>='a' && s[i]<='z') {
            digit = s[i]-'a' + 26;
        } else if (s[i]>='0' && s[i]<='9') {
            digit = s[i]-'0' + 52;
        } else if (s[i]=='+') {
            digit = 62;
        } else if (s[i]=='/') {
            digit = 63;
        } else {
            continue;
        }

        // only reached for valid base64 characters
        val |= IntegerOps::shiftLeft2(digit, 18-6*ndigits);
        if (4==++ndigits) {
            retval.push_back(IntegerOps::shiftRightLogical2(val, 16) & 0xff);
            retval.push_back(IntegerOps::shiftRightLogical2(val, 8) & 0xff);
            retval.push_back(val & 0xff);
            val = ndigits = 0;
        }
    }

    assert(0==ndigits || 2==ndigits || 3==ndigits);
    if (ndigits>=2) {
        retval.push_back(IntegerOps::shiftRightLogical2(val, 16) & 0xff);
        if (ndigits==3)
            retval.push_back(IntegerOps::shiftRightLogical2(val, 8) & 0xff);
    }
    return retval;
}

// DQ (2/18/2006): Added general name mangling for all declarations (and some other IR nodes).
// JJW (10/15/2007): Does this compute a ones-complement checksum like used for TCP?
unsigned long
generate_checksum(std::string s) {
     std::string uniqueName = s;

  // The checksum function requires a even length string (so we have to fix it up)
     if (uniqueName.size() % 2 != 0)
        {
       // printf ("Adding another character to make string an even valued length \n");
       // Use a character that does not appear in mangled 
       // names so that no other mangled name could include it.
          uniqueName += "#";
        }
     ROSE_ASSERT(uniqueName.size() % 2 == 0);

  // Call a simple checksum function
  // unsigned short int checksum = StringUtility::chksum(buffer,uniqueName.size());

     unsigned long accum = 0;
     unsigned long chksm;

     accum = 0;
     unsigned int len  = uniqueName.size() / 2; /* Words only */
     for (unsigned int i=0; i< len; i++)
          accum += (unsigned long) (((unsigned short*)uniqueName.data())[i]);

     chksm = (accum & 0xffff); /* Mask all but low word */
     chksm += (accum >> 16); /* Sum all the carries */

     if (chksm > 0xffff) /* If this also carried */
          chksm++; /* Sum this too */
     unsigned short checksum = (unsigned short) (chksm & 0xffff);

  // printf ("Checksum = %d \n",checksum);
     return (unsigned long) checksum;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
isLineTerminated(const std::string &s) {
    return !s.empty() && ('\n'==s[s.size()-1] || '\r'==s[s.size()-1]);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions related to diagnostic messages
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
indentMultilineString(const std::string& inputString, int statementColumnNumber) {
  // Indent the transformation to match the statement that it is transforming

     std::string returnString;

  // Put in linefeeds to avoid endless recursion in the copyEdit (I think)
     ROSE_ASSERT (statementColumnNumber > 0);
     std::string cr_and_added_space = std::string(statementColumnNumber, ' ');
     cr_and_added_space[0] = '\t';

  // returnString = copyEdit (inputString,"\n",cr_and_added_space);
     returnString = copyEdit (inputString,"\n",cr_and_added_space);

  // Now exchange the line feeds for carriage returns
  // returnString = copyEdit (returnString,"\t","\n");
     returnString = copyEdit (returnString,"\t","\n");

  // Now indent the first line (since there was no CR) there
     returnString = cr_and_added_space.substr(1) + returnString;

  // printf ("In StringUtility::indentMultilineString(): returnString = %s \n",returnString);
     return returnString;
}

void
add_to_reason_string(std::string &result, bool isset, bool do_pad, const std::string &abbr, const std::string &full) {
    if (isset) {
        if (do_pad) {
            result += abbr;
        } else {
            if (result.size()>0) result += ", ";
            result += full;
        }
    } else if (do_pad) {
        for (size_t i=0; i<abbr.size(); ++i)
            result += ".";
    }
}

std::string
appendAsmComment(const std::string &s, const std::string &comment) {
    if (comment.empty())
        return s;
    if (s.empty())
        return "<" + comment + ">";
    if (s[s.size()-1] == '>')
        return s.substr(0, s.size()-1) + "," + comment + ">";
    return s + "<" + comment + ">";
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Strings parsed from text files
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
StringWithLineNumber::toString() const {
     std::ostringstream os;

  // DQ (1/21/2010): Added support for skipping these when in makes it easer to debug ROSETTA generated files.
#ifdef SKIP_HASH_LINE_NUMBER_DECLARATIONS_IN_GENERATED_FILES
  // os << str << std::endl;
     os << "/* #line " << line << " \""
        << (filename.empty() ? "" : getAbsolutePathFromRelativePath(filename)) << "\" */\n" << str << std::endl;
#else
     os << "#line " << line << " \""
        << (filename.empty() ? "" : getAbsolutePathFromRelativePath(filename)) << "\"\n" << str << std::endl;
#endif

     return os.str();
}

std::string
toString(const FileWithLineNumbers& strings, const std::string& filename, int physicalLine /* Line number in output file */) {
  std::string result;
  unsigned int lastLineNumber = 1;
  std::string lastFile = "";
  bool inPhysicalFile = true; // Not in a specifically named file
  bool needLineDirective = false;
  for (unsigned int i = 0; i < strings.size(); ++i) {
    // Determine if a #line directive is needed, if the last iteration did not
    // force one to be added
    bool newInPhysicalFile = (strings[i].filename == "");
    if (inPhysicalFile != newInPhysicalFile)
      needLineDirective = true;
    if (!inPhysicalFile &&
        (strings[i].filename != lastFile ||
         strings[i].line != lastLineNumber))
      needLineDirective = true;

    if (strings[i].str == "" && i + 1 == strings.size()) { // Special case
      needLineDirective = false;
    }

    // Print out the #line directive (if needed) and the actual line
    if (needLineDirective) {
#ifdef SKIP_HASH_LINE_NUMBER_DECLARATIONS_IN_GENERATED_FILES
           /* Nothing to do here! */
      if (strings[i].filename == "") { // Reset to actual input file
          // The "1" is the increment because number is the line number of the NEXT line after the #line directive
          result += "/* #line " + numberToString(physicalLine + 1) + " \"" + filename + "\" */\n";
      } else {
          result += "/* #line " + numberToString(strings[i].line) + " \"" + strings[i].filename + "\" */\n";
      }
#else
      if (strings[i].filename == "") { // Reset to actual input file
          // The "1" is the increment because number is the line number of the NEXT line after the #line directive
          result += "#line " + numberToString(physicalLine + 1) + " \"" + filename + "\"\n";
      } else {
          result += "#line " + numberToString(strings[i].line) + " \"" + strings[i].filename + "\"\n";
      }
#endif
      // These are only updated when a #line directive is actually printed,
      // largely because of the blank line exception above (so if a blank line
      // starts a new file, the #line directive needs to be emitted on the
      // first non-blank line)
      lastLineNumber = strings[i].line + 1;
      lastFile = strings[i].filename;
    } else {
      ++lastLineNumber;
    }
    result += strings[i].str + "\n";

    bool printedLineDirective = needLineDirective;

    // Determine if a #line directive is needed for the next iteration
    needLineDirective = false;
    inPhysicalFile = newInPhysicalFile;
    if (strings[i].str.find('\n') != std::string::npos && !inPhysicalFile) {
      needLineDirective = true; // Ensure that the next line has a #line directive
    }

    // Update the physical line counter based on the number of lines output
    if (printedLineDirective) ++physicalLine; // For #line directive
    for (size_t pos = strings[i].str.find('\n');
         pos != std::string::npos; pos = strings[i].str.find('\n', pos + 1)) {
      ++physicalLine; // Increment for \n in string
    }
    ++physicalLine; // Increment for \n added at end of line
  }
  return result;
}

// BP : 10/25/2001, a non recursive version that allocs memory only once
std::string
copyEdit(const std::string& inputString, const std::string& oldToken, const std::string& newToken) {
    std::string returnString;
    std::string::size_type oldTokenSize = oldToken.size();

    std::string::size_type position = 0;
    std::string::size_type lastPosition = 0;
    while (true) {
        position = inputString.find(oldToken, position);
        if (position == std::string::npos) {
            returnString += inputString.substr(lastPosition);
            break;
        } else {
            returnString += inputString.substr(lastPosition, position - lastPosition);
            returnString += newToken;
            position = lastPosition = position + oldTokenSize;
        }
    }

    return returnString;
}

FileWithLineNumbers
copyEdit(const FileWithLineNumbers& inputString, const std::string& oldToken, const std::string& newToken) {
    FileWithLineNumbers result = inputString;
    for (unsigned int i = 0; i < result.size(); ++i)
        result[i].str = copyEdit(result[i].str, oldToken, newToken);
    return result;
}

FileWithLineNumbers
copyEdit(const FileWithLineNumbers& inputString, const std::string& oldToken, const FileWithLineNumbers& newToken ) {
  FileWithLineNumbers result = inputString;
  for (unsigned int i = 0; i < result.size(); ++i) {
    std::string str = result[i].str;
    std::string::size_type pos = str.find(oldToken);
    if (pos != std::string::npos) {
      // Split the line into the before-substitution and after-substitution regions
      result[i].str = str.substr(0, pos);
      result.insert(result.begin() + i + 1,
                    StringWithLineNumber(str.substr(pos + oldToken.size()),
                                         result[i].filename + " after subst for " + oldToken,
                                         result[i].line));
      // Do the insertion
      result.insert(result.begin() + i + 1, newToken.begin(), newToken.end());
      i += newToken.size(); // Rescan the after-substitution part of the old line, but not any of the new text
    }
  }
  return result;
}

} // namespace
} // namespace
