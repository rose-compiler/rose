#include <Rose/StringUtility/Convert.h>

#include <Rose/StringUtility/Predicate.h>
#include <Rose/StringUtility/SplitJoin.h>
#include <integerOps.h>                                 // rose

#include <boost/regex.hpp>
#include <cassert>
#include <list>

namespace Rose {
namespace StringUtility {

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
leftJustify(const std::string &s, size_t width, char fill) {
    if (s.empty()) {
        return std::string(width, fill);
    } else if (s.size() >= width) {
        return s.substr(0, width);
    } else {
        return s + std::string(width - s.size(), fill);
    }
}

std::string
rightJustify(const std::string &s, size_t width, char fill) {
    if (s.empty()) {
        return std::string(width, fill);
    } else if (s.size() >= width) {
        return s.substr(0, width);
    } else {
        return std::string(width - s.size(), fill) + s;
    }
}

std::string
centerJustify(const std::string &s, size_t width, char fill) {
    if (s.empty()) {
        return std::string(width, fill);
    } else if (s.size() >= width) {
        return s.substr(0, width);
    } else {
        size_t nl = (width - s.size()) / 2;
        size_t nr = (width - s.size()) - nl;            // if odd, put more space on the right side
        return std::string(nl, fill) + s + std::string(nr, fill);
    }
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

std::string
removeAnsiEscapes(const std::string &s) {
    boost::regex csiSequences("\\033\\[[\\x30-\\x3f]*[\\x20-\\x2f]*[\\x40-x7e]");
    std::string retval;

    const char *iter = s.c_str();
    const char *end = s.c_str() + s.size();
    boost::cmatch found;
    while (boost::regex_search(iter, end, found, csiSequences)) {
        retval += std::string(iter, iter + found.position());
        iter += found.position() + found.length();
    }

    if (iter != end)
        retval += std::string(iter, end);
    return retval;
}

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
     assert(uniqueName.size() % 2 == 0);

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

} // namespace
} // namespace
